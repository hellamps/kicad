/**
 * @file edit_track_width.cpp
 * @brief Functions to modify sizes of segment, track, net, all vias and/or all tracks.
 */

#include <fctsys.h>
#include <gr_basic.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <wxPcbStruct.h>

#include <class_board.h>
#include <class_track.h>

#include <pcbnew.h>
#include <drc_stuff.h>
#include <protos.h>


/**
 * Function SetTrackSegmentWidth
 *  Modify one track segment width or one via diameter and drill (using DRC control).
 *  Basic routine used by other routines when editing tracks or vias
 * @param aTrackItem = the track segment or via to modify
 * @param aItemsListPicker = the list picker to use for an undo command (can be NULL)
 * @param aUseNetclassValue = true to use NetClass value, false to use BOARD::m_designSettings value
 * @return  true if done, false if no not change (because DRC error)
 */
bool PCB_EDIT_FRAME::SetTrackSegmentWidth( TRACK*             aTrackItem,
                                           PICKED_ITEMS_LIST* aItemsListPicker,
                                           bool               aUseNetclassValue )
{
    int           initial_width, new_width;
    int           initial_drill = -1,new_drill = -1;
    bool          change_ok = false;
    NETINFO_ITEM* net = NULL;

    if( aUseNetclassValue )
        net = GetBoard()->FindNet( aTrackItem->GetNet() );

    initial_width = aTrackItem->m_Width;

    if( net )
        new_width = net->GetTrackWidth();
    else
        new_width = GetBoard()->GetCurrentTrackWidth();

    if( aTrackItem->Type() == PCB_VIA_T )
    {
        if( !aTrackItem->IsDrillDefault() )
            initial_drill = aTrackItem->GetDrillValue();

        if( net )
        {
            new_width = net->GetViaSize();
        }
        else
        {
            new_width = GetBoard()->GetCurrentViaSize();
            new_drill = GetBoard()->GetCurrentViaDrill();
        }

        if( aTrackItem->m_Shape == VIA_MICROVIA )
        {
            if( net )
                new_width = net->GetViaSize();
            else
                new_width = net->GetMicroViaSize();
        }
    }

    aTrackItem->m_Width = new_width;

    // make a DRC test because the new size is bigger than the old size
    if( initial_width < new_width )
    {
        int diagdrc = OK_DRC;

        if( Drc_On )
            diagdrc = m_drc->Drc( aTrackItem, GetBoard()->m_Track );

        if( diagdrc == OK_DRC )
            change_ok = true;
    }
    else if( initial_width > new_width )
    {
        change_ok = true;
    }
    else if( (aTrackItem->Type() == PCB_VIA_T) && (initial_drill != new_drill) )
    {
        // if new width == initial_width: do nothing, unless a via has its drill value changed
        change_ok = true;
    }

    if( change_ok )
    {
        OnModify();

        if( aItemsListPicker )
        {
            aTrackItem->m_Width = initial_width;
            ITEM_PICKER picker( aTrackItem, UR_CHANGED );
            picker.SetLink( aTrackItem->Clone() );
            aItemsListPicker->PushItem( picker );
            aTrackItem->m_Width = new_width;

            if( aTrackItem->Type() == PCB_VIA_T )
            {
                // Set new drill value. Note: currently microvias have only a default drill value
                if( new_drill > 0 )
                    aTrackItem->SetDrill( new_drill );
                else
                    aTrackItem->SetDrillDefault();
            }
        }
    }
    else
    {
        aTrackItem->m_Width = initial_width;
    }

    return change_ok;
}


/**
 * Function Edit_TrackSegm_Width
 * Modify one track segment width or one via diameter (using DRC control).
 * @param aDC = the curred device context (can be NULL)
 * @param aTrackItem = the track segment or via to modify
 */
void PCB_EDIT_FRAME::Edit_TrackSegm_Width( wxDC* aDC, TRACK* aTrackItem )
{
    PICKED_ITEMS_LIST itemsListPicker;
    bool change = SetTrackSegmentWidth( aTrackItem, &itemsListPicker, false );

    if( change == 0 || aTrackItem->GetFlags() )
        return;     // No change

    // The segment has changed: redraw it and save it in undo list
    if( aDC )
    {
        TRACK* oldsegm = (TRACK*) itemsListPicker.GetPickedItemLink( 0 );
        wxASSERT( oldsegm );
        m_canvas->CrossHairOff( aDC );                  // Erase cursor shape
        oldsegm->Draw( m_canvas, aDC, GR_XOR );         // Erase old track shape
        aTrackItem->Draw( m_canvas, aDC, GR_OR );       // Display new track shape
        m_canvas->CrossHairOn( aDC );                   // Display cursor shape
    }

    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );
}


/**
 * Function Edit_Track_Width
 * Modify a full track width (using DRC control).
 * a full track is the set of track segments between 2 ends: pads or a point that has
 * more than 2 segments ends connected
 * @param aDC = the curred device context (can be NULL)
 * @param aTrackSegment = a segment or via on the track to change
 */
void PCB_EDIT_FRAME::Edit_Track_Width( wxDC* aDC, TRACK* aTrackSegment )
{
    TRACK* pt_track;
    int    nb_segm;

    if( aTrackSegment == NULL )
        return;

    pt_track = GetBoard()->MarkTrace( aTrackSegment, &nb_segm, NULL, NULL, true );

    PICKED_ITEMS_LIST itemsListPicker;
    bool change = false;

    for( int ii = 0; ii < nb_segm; ii++, pt_track = pt_track->Next() )
    {
        pt_track->SetState( BUSY, OFF );

        if( SetTrackSegmentWidth( pt_track, &itemsListPicker, false ) )
            change = true;
    }

    if( !change )
        return;

    // Some segment have changed: redraw them and save in undo list
    if( aDC )
    {
        m_canvas->CrossHairOff( aDC );                     // Erase cursor shape

        for( unsigned ii = 0; ii < itemsListPicker.GetCount(); ii++ )
        {
            TRACK* segm = (TRACK*) itemsListPicker.GetPickedItemLink( ii );
            segm->Draw( m_canvas, aDC, GR_XOR );            // Erase old track shape
            segm = (TRACK*) itemsListPicker.GetPickedItem( ii );
            segm->Draw( m_canvas, aDC, GR_OR );             // Display new track shape
        }

        m_canvas->CrossHairOn( aDC );                   // Display cursor shape
    }

    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );
}


/**
 * Function Change_Net_Tracks_And_Vias_Sizes
 * Reset all tracks width and vias diameters and drill
 * to their default Netclass value or current values
 * @param aNetcode : the netcode of the net to edit
 * @param aUseNetclassValue : bool. True to use netclass values, false to use current values
 */
bool PCB_EDIT_FRAME::Change_Net_Tracks_And_Vias_Sizes( int aNetcode, bool aUseNetclassValue )
{
    TRACK* pt_segm;

    if( aNetcode <= 0 )
        return false;

    // Examine segments
    PICKED_ITEMS_LIST itemsListPicker;
    bool change = false;

    for( pt_segm = GetBoard()->m_Track; pt_segm != NULL; pt_segm = pt_segm->Next() )
    {
        if( aNetcode != pt_segm->GetNet() )         // not in net
            continue;

        // we have found a item member of the net
        if( SetTrackSegmentWidth( pt_segm, &itemsListPicker, aUseNetclassValue ) )
            change = true;
    }

    if( !change )
        return false;

    // Some segment have changed: save them in undo list
    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );
    return true;
}


bool PCB_EDIT_FRAME::Reset_All_Tracks_And_Vias_To_Netclass_Values( bool aTrack, bool aVia )
{
    TRACK* pt_segm;

    // read and edit tracks and vias if required
    PICKED_ITEMS_LIST itemsListPicker;
    bool change = false;

    for( pt_segm = GetBoard()->m_Track; pt_segm != NULL; pt_segm = pt_segm->Next() )
    {
        if( (pt_segm->Type() == PCB_VIA_T ) && aVia )
        {
            if( SetTrackSegmentWidth( pt_segm, &itemsListPicker, true ) )
                change = true;
        }

        if( (pt_segm->Type() == PCB_TRACE_T ) && aTrack )
        {
            if( SetTrackSegmentWidth( pt_segm, &itemsListPicker, true ) )
                change = true;
        }
    }

    if( !change )
        return false;

    // Some segment have changed: save them in undo list
    SaveCopyInUndoList( itemsListPicker, UR_CHANGED );

    return true;
}
