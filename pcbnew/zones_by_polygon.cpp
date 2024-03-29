/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2007 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2011 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 1992-2011 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/**
 * @file zones_by_polygon.cpp
 */

#include <fctsys.h>
#include <appl_wxstruct.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <wxPcbStruct.h>

#include <class_board.h>
#include <class_zone.h>

#include <pcbnew.h>
#include <zones.h>
#include <pcbnew_id.h>
#include <protos.h>
#include <zones_functions_for_undo_redo.h>
#include <drc_stuff.h>

bool s_Verbose = false;       // false if zone outline diags must not be shown

// Outline creation:
static void Abort_Zone_Create_Outline( EDA_DRAW_PANEL* Panel, wxDC* DC );
static void Show_New_Edge_While_Move_Mouse( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
                                            const wxPoint& aPosition, bool aErase );

// Corner moving
static void Abort_Zone_Move_Corner_Or_Outlines( EDA_DRAW_PANEL* Panel, wxDC* DC );
static void Show_Zone_Corner_Or_Outline_While_Move_Mouse( EDA_DRAW_PANEL* aPanel,
                                                          wxDC*           aDC,
                                                          const wxPoint&  aPosition,
                                                          bool            aErase );

// Local variables
static wxPoint         s_CornerInitialPosition;     // Used to abort a move corner command
static bool            s_CornerIsNew;               // Used to abort a move corner command (if it is a new corner, it must be deleted)
static bool            s_AddCutoutToCurrentZone;    // if true, the next outline will be added to s_CurrentZone
static ZONE_CONTAINER* s_CurrentZone;               // if != NULL, these ZONE_CONTAINER params will be used for the next zone
static wxPoint         s_CursorLastPosition;        // in move zone outline, last cursor position. Used to calculate the move vector
static PICKED_ITEMS_LIST s_PickedList;              // a picked list to save zones for undo/redo command
static PICKED_ITEMS_LIST _AuxiliaryList;            // a picked list to store zones that are deleted or added when combined


void PCB_EDIT_FRAME::Add_Similar_Zone( wxDC* DC, ZONE_CONTAINER* aZone )
{
    if( !aZone )
        return;

    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = aZone;

    // set zone settings to the current zone
    ZONE_SETTINGS  zoneInfo = GetZoneSettings();
    zoneInfo << *aZone;
    SetZoneSettings( zoneInfo );

    // Use the general event handler to set others params (like toolbar)
    wxCommandEvent evt;
    evt.SetId( ID_PCB_ZONES_BUTT );
    OnSelectTool( evt );
}


void PCB_EDIT_FRAME::Add_Zone_Cutout( wxDC* DC, ZONE_CONTAINER* aZone )
{
    if( !aZone )
        return;

    s_AddCutoutToCurrentZone = true;
    s_CurrentZone = aZone;

    // set zones setup to the current zone
    ZONE_SETTINGS zoneInfo = GetZoneSettings();
    zoneInfo << *aZone;
    SetZoneSettings( zoneInfo );

    // Use the general event handle to set others params (like toolbar)
    wxCommandEvent evt;
    evt.SetId( ID_PCB_ZONES_BUTT );
    OnSelectTool( evt );
}


int PCB_EDIT_FRAME::Delete_LastCreatedCorner( wxDC* DC )
{
    ZONE_CONTAINER* zone = GetBoard()->m_CurrentZoneContour;

    if( !zone )
        return 0;

    if( !zone->GetNumCorners() )
        return 0;

    zone->DrawWhileCreateOutline( m_canvas, DC, GR_XOR );

    if( zone->GetNumCorners() > 2 )
    {
        zone->m_Poly->DeleteCorner( zone->GetNumCorners() - 1 );

        if( m_canvas->IsMouseCaptured() )
            m_canvas->CallMouseCapture( DC, wxDefaultPosition, false );
    }
    else
    {
        m_canvas->SetMouseCapture( NULL, NULL );
        SetCurItem( NULL );
        zone->RemoveAllContours();
        zone->ClearFlags();
    }

    return zone->GetNumCorners();
}


/**
 * Function Abort_Zone_Create_Outline
 * cancels the Begin_Zone command if at least one EDGE_ZONE was created.
 */
static void Abort_Zone_Create_Outline( EDA_DRAW_PANEL* Panel, wxDC* DC )
{
    PCB_EDIT_FRAME* pcbframe = (PCB_EDIT_FRAME*) Panel->GetParent();
    ZONE_CONTAINER* zone = pcbframe->GetBoard()->m_CurrentZoneContour;

    if( zone )
    {
        zone->DrawWhileCreateOutline( Panel, DC, GR_XOR );
        zone->ClearFlags();
        zone->RemoveAllContours();
    }

    pcbframe->SetCurItem( NULL );
    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;
    Panel->SetMouseCapture( NULL, NULL );
}


void PCB_EDIT_FRAME::Start_Move_Zone_Corner( wxDC* DC, ZONE_CONTAINER* aZone,
                                             int corner_id, bool IsNewCorner )
{
    if( aZone->IsOnCopperLayer() ) // Show the Net
    {
        if( GetBoard()->IsHighLightNetON() && DC )
        {
            HighLight( DC );  // Remove old highlight selection
        }

        ZONE_SETTINGS zoneInfo = GetZoneSettings();
        zoneInfo.m_NetcodeSelection = aZone->GetNet();
        SetZoneSettings( zoneInfo );

        GetBoard()->SetHighLightNet( aZone->GetNet() );

        if( DC )
            HighLight( DC );
    }


    // Prepare copy of old zones, for undo/redo.
    // if the corner is new, remove it from list, save and insert it in list
    int cx = aZone->m_Poly->GetX( corner_id );
    int cy = aZone->m_Poly->GetY( corner_id );

    if ( IsNewCorner )
        aZone->m_Poly->DeleteCorner( corner_id );

    _AuxiliaryList.ClearListAndDeleteItems();
    s_PickedList.ClearListAndDeleteItems();

    SaveCopyOfZones( s_PickedList, GetBoard(), aZone->GetNet(),
                     aZone->GetLayer() );

    if ( IsNewCorner )
        aZone->m_Poly->InsertCorner(corner_id-1, cx, cy );

    aZone->SetFlags( IN_EDIT );
    m_canvas->SetMouseCapture( Show_Zone_Corner_Or_Outline_While_Move_Mouse,
                                Abort_Zone_Move_Corner_Or_Outlines );
    s_CornerInitialPosition = aZone->GetCornerPosition( corner_id );
    s_CornerIsNew = IsNewCorner;
    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;
}


void PCB_EDIT_FRAME::Start_Move_Zone_Drag_Outline_Edge( wxDC*           DC,
                                                        ZONE_CONTAINER* aZone,
                                                        int             corner_id )
{
    aZone->SetFlags( IS_DRAGGED );
    aZone->m_CornerSelection = corner_id;
    m_canvas->SetMouseCapture( Show_Zone_Corner_Or_Outline_While_Move_Mouse,
                                Abort_Zone_Move_Corner_Or_Outlines );
    s_CursorLastPosition     = s_CornerInitialPosition = GetScreen()->GetCrossHairPosition();
    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;

    s_PickedList.ClearListAndDeleteItems();
    _AuxiliaryList.ClearListAndDeleteItems();
    SaveCopyOfZones( s_PickedList, GetBoard(), aZone->GetNet(),
                     aZone->GetLayer() );
}


void PCB_EDIT_FRAME::Start_Move_Zone_Outlines( wxDC* DC, ZONE_CONTAINER* aZone )
{
    // Show the Net
    if( aZone->IsOnCopperLayer() ) // Show the Net
    {
        if( GetBoard()->IsHighLightNetON() )
        {
            HighLight( DC );  // Remove old highlight selection
        }

        ZONE_SETTINGS zoneInfo = GetZoneSettings();
        zoneInfo.m_NetcodeSelection = aZone->GetNet();
        SetZoneSettings( zoneInfo );

        GetBoard()->SetHighLightNet( aZone->GetNet() );
        HighLight( DC );
    }

    s_PickedList.ClearListAndDeleteItems();
    _AuxiliaryList.ClearListAndDeleteItems();
    SaveCopyOfZones( s_PickedList, GetBoard(), aZone->GetNet(),
                     aZone->GetLayer() );

    aZone->SetFlags( IS_MOVED );
    m_canvas->SetMouseCapture( Show_Zone_Corner_Or_Outline_While_Move_Mouse,
                                Abort_Zone_Move_Corner_Or_Outlines );
    s_CursorLastPosition = s_CornerInitialPosition = GetScreen()->GetCrossHairPosition();
    s_CornerIsNew = false;
    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;
}


void PCB_EDIT_FRAME::End_Move_Zone_Corner_Or_Outlines( wxDC* DC, ZONE_CONTAINER* aZone )
{
    aZone->ClearFlags();
    m_canvas->SetMouseCapture( NULL, NULL );

    if( DC )
        aZone->Draw( m_canvas, DC, GR_OR );

    OnModify();
    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;

    SetCurItem( NULL );       // This outline can be deleted when merging outlines

    // Combine zones if possible
    wxBusyCursor dummy;
    GetBoard()->AreaPolygonModified( &_AuxiliaryList, aZone, true, s_Verbose );
    m_canvas->Refresh();


    int ii = GetBoard()->GetAreaIndex( aZone );     // test if aZone exists

    if( ii < 0 )
        aZone = NULL;                          // was removed by combining zones

    UpdateCopyOfZonesList( s_PickedList, _AuxiliaryList, GetBoard() );
    SaveCopyInUndoList(s_PickedList, UR_UNSPECIFIED);
    s_PickedList.ClearItemsList(); // s_ItemsListPicker is no more owner of picked items

    int error_count = GetBoard()->Test_Drc_Areas_Outlines_To_Areas_Outlines( aZone, true );

    if( error_count )
    {
        DisplayError( this, _( "Area: DRC outline error" ) );
    }
}


void PCB_EDIT_FRAME::Remove_Zone_Corner( wxDC* DC, ZONE_CONTAINER* aZone )
{
    OnModify();

    if( aZone->m_Poly->GetNumCorners() <= 3 )
    {
        m_canvas->RefreshDrawingRect( aZone->GetBoundingBox() );

        if( DC )
        {  // Remove the full zone because this is no more an area
            aZone->UnFill();
            aZone->DrawFilledArea( m_canvas, DC, GR_XOR );
        }

        GetBoard()->Delete( aZone );
        return;
    }

    int layer = aZone->GetLayer();

    if( DC )
    {
        GetBoard()->RedrawAreasOutlines( m_canvas, DC, GR_XOR, layer );
        GetBoard()->RedrawFilledAreas( m_canvas, DC, GR_XOR, layer );
    }

    _AuxiliaryList.ClearListAndDeleteItems();
    s_PickedList. ClearListAndDeleteItems();
    SaveCopyOfZones( s_PickedList, GetBoard(), aZone->GetNet(),
                     aZone->GetLayer() );
    aZone->m_Poly->DeleteCorner( aZone->m_CornerSelection );

    // modify zones outlines according to the new aZone shape
    GetBoard()->AreaPolygonModified( &_AuxiliaryList, aZone, true, s_Verbose );

    if( DC )
    {
        GetBoard()->RedrawAreasOutlines( m_canvas, DC, GR_OR, layer );
        GetBoard()->RedrawFilledAreas( m_canvas, DC, GR_OR, layer );
    }

    UpdateCopyOfZonesList( s_PickedList, _AuxiliaryList, GetBoard() );
    SaveCopyInUndoList(s_PickedList, UR_UNSPECIFIED);
    s_PickedList.ClearItemsList(); // s_ItemsListPicker is no more owner of picked items

    int ii = GetBoard()->GetAreaIndex( aZone );     // test if aZone exists

    if( ii < 0 )
        aZone = NULL;   // aZone does not exist anymore, after combining zones

    int error_count = GetBoard()->Test_Drc_Areas_Outlines_To_Areas_Outlines( aZone, true );

    if( error_count )
    {
        DisplayError( this, _( "Area: DRC outline error" ) );
    }
}


/**
 * Function Abort_Zone_Move_Corner_Or_Outlines
 * cancels the Begin_Zone state if at least one EDGE_ZONE has been created.
 */
void Abort_Zone_Move_Corner_Or_Outlines( EDA_DRAW_PANEL* Panel, wxDC* DC )
{
    PCB_EDIT_FRAME* pcbframe = (PCB_EDIT_FRAME*) Panel->GetParent();
    ZONE_CONTAINER* zone     = (ZONE_CONTAINER*) pcbframe->GetCurItem();

    if( zone->IsMoving() )
    {
        wxPoint offset;
        offset = s_CornerInitialPosition - s_CursorLastPosition;
        zone->Move( offset );
    }
    else if( zone->IsDragging() )
    {
        wxPoint offset;
        offset = s_CornerInitialPosition - s_CursorLastPosition;
        zone->MoveEdge( offset );
    }
    else
    {
        if( s_CornerIsNew )
        {
            zone->m_Poly->DeleteCorner( zone->m_CornerSelection );
        }
        else
        {
            wxPoint pos = s_CornerInitialPosition;
            zone->m_Poly->MoveCorner( zone->m_CornerSelection, pos.x, pos.y );
        }
    }

    Panel->SetMouseCapture( NULL, NULL );
    _AuxiliaryList.ClearListAndDeleteItems();
    s_PickedList. ClearListAndDeleteItems();
    Panel->Refresh();

    pcbframe->SetCurItem( NULL );
    zone->ClearFlags();
    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;
}


/// Redraws the zone outline when moving a corner according to the cursor position
void Show_Zone_Corner_Or_Outline_While_Move_Mouse( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
                                                   const wxPoint& aPosition, bool aErase )
{
    PCB_EDIT_FRAME* pcbframe = (PCB_EDIT_FRAME*) aPanel->GetParent();
    ZONE_CONTAINER* zone = (ZONE_CONTAINER*) pcbframe->GetCurItem();

    if( aErase )    // Undraw edge in old position
    {
        zone->Draw( aPanel, aDC, GR_XOR );
    }

    wxPoint pos = pcbframe->GetScreen()->GetCrossHairPosition();

    if( zone->IsMoving() )
    {
        wxPoint offset;
        offset = pos - s_CursorLastPosition;
        zone->Move( offset );
        s_CursorLastPosition = pos;
    }
    else if( zone->IsDragging() )
    {
        wxPoint offset;
        offset = pos - s_CursorLastPosition;
        zone->MoveEdge( offset );
        s_CursorLastPosition = pos;
    }
    else
    {
        zone->m_Poly->MoveCorner( zone->m_CornerSelection, pos.x, pos.y );
    }

    zone->Draw( aPanel, aDC, GR_XOR );
}



int PCB_EDIT_FRAME::Begin_Zone( wxDC* DC )
{
    ZONE_SETTINGS zoneInfo = GetZoneSettings();

    // verify if s_CurrentZone exists (could be deleted since last selection) :
    int ii;
    for( ii = 0; ii < GetBoard()->GetAreaCount(); ii++ )
    {
        if( s_CurrentZone == GetBoard()->GetArea( ii ) )
            break;
    }

    if( ii >= GetBoard()->GetAreaCount() ) // Not found: could be deleted since last selection
    {
        s_AddCutoutToCurrentZone = false;
        s_CurrentZone = NULL;
    }

    // If no zone contour in progress, a new zone is being created:
    if( !GetBoard()->m_CurrentZoneContour )
        GetBoard()->m_CurrentZoneContour = new ZONE_CONTAINER( GetBoard() );

    ZONE_CONTAINER* zone = GetBoard()->m_CurrentZoneContour;

    if( zone->GetNumCorners() == 0 )    // Start a new contour: init zone params (net, layer ...)
    {
        if( !s_CurrentZone )            // A new outline is created, from scratch
        {
            ZONE_EDIT_T edited;

            // Init zone params to reasonable values
            zone->SetLayer( getActiveLayer() );

            // Prompt user for parameters:
            m_canvas->SetIgnoreMouseEvents( true );

            if( zone->IsOnCopperLayer() )
            {
                // Put a zone on a copper layer
                if( GetBoard()->GetHighLightNetCode() > 0 )
                {
                    zoneInfo.m_NetcodeSelection = GetBoard()->GetHighLightNetCode();

                    zone->SetNet( zoneInfo.m_NetcodeSelection );
                    zone->SetNetNameFromNetCode( );
                }

                wxGetApp().GetSettings()->Read( ZONE_THERMAL_RELIEF_GAP_STRING_KEY,
                                                &zoneInfo.m_ThermalReliefGap );

                wxGetApp().GetSettings()->Read( ZONE_THERMAL_RELIEF_COPPER_WIDTH_STRING_KEY,
                                                &zoneInfo.m_ThermalReliefCopperBridge );

                zoneInfo.m_CurrentZone_Layer = zone->GetLayer();

                edited = InvokeCopperZonesEditor( this, &zoneInfo );
            }
            else   // Put a zone on a non copper layer (technical layer)
            {
                zoneInfo.m_NetcodeSelection = 0;     // No net for non copper zones

                edited = InvokeNonCopperZonesEditor( this, zone, &zoneInfo );
            }

            m_canvas->MoveCursorToCrossHair();
            m_canvas->SetIgnoreMouseEvents( false );

            if( edited == ZONE_ABORT )
                return 0;

            // Switch active layer to the selected zone layer
            setActiveLayer( zoneInfo.m_CurrentZone_Layer );

            SetZoneSettings( zoneInfo );
        }
        else
        {
            // Start a new contour: init zone params (net and layer) from an existing
            // zone (add cutout or similar zone)

            zoneInfo.m_CurrentZone_Layer = s_CurrentZone->GetLayer();
            setActiveLayer( s_CurrentZone->GetLayer() );

            zoneInfo << *s_CurrentZone;

            SetZoneSettings( zoneInfo );
        }

        // Show the Net for zones on copper layers
        if( zoneInfo.m_CurrentZone_Layer < FIRST_NO_COPPER_LAYER )
        {
            if( s_CurrentZone )
            {
                zoneInfo.m_NetcodeSelection = s_CurrentZone->GetNet();
                GetBoard()->SetZoneSettings( zoneInfo );
            }

            if( GetBoard()->IsHighLightNetON() )
            {
                HighLight( DC );    // Remove old highlight selection
            }

            GetBoard()->SetHighLightNet( zoneInfo.m_NetcodeSelection );
            HighLight( DC );
        }

        if( !s_AddCutoutToCurrentZone )
            s_CurrentZone = NULL; // the zone is used only once ("add similar zone" command)
    }

    // if first segment
    if( zone->GetNumCorners() == 0 )
    {
        zone->SetFlags( IS_NEW );
        zone->SetTimeStamp( GetNewTimeStamp() );

        zoneInfo.ExportSetting( *zone );

        zone->m_Poly->Start( zoneInfo.m_CurrentZone_Layer,
                             GetScreen()->GetCrossHairPosition().x,
                             GetScreen()->GetCrossHairPosition().y,
                             zone->GetHatchStyle() );

        zone->AppendCorner( GetScreen()->GetCrossHairPosition() );

        if( Drc_On && (m_drc->Drc( zone, 0 ) == BAD_DRC) && zone->IsOnCopperLayer() )
        {
            zone->ClearFlags();
            zone->RemoveAllContours();

            // use the form of SetCurItem() which does not write to the msg panel,
            // SCREEN::SetCurItem(), so the DRC error remains on screen.
            // PCB_EDIT_FRAME::SetCurItem() calls DisplayInfo().
            GetScreen()->SetCurItem( NULL );
            DisplayError( this,
                          _( "DRC error: this start point is inside or too close an other area" ) );
            return 0;
        }

        SetCurItem( zone );
        m_canvas->SetMouseCapture( Show_New_Edge_While_Move_Mouse, Abort_Zone_Create_Outline );
    }
    else    // edge in progress:
    {
        ii = zone->GetNumCorners() - 1;

        // edge in progress : the current corner coordinate was set by Show_New_Edge_While_Move_Mouse
        if( zone->GetCornerPosition( ii - 1 ) != zone->GetCornerPosition( ii ) )
        {
            if( !Drc_On || !zone->IsOnCopperLayer() || ( m_drc->Drc( zone, ii - 1 ) == OK_DRC ) )
            {
                // Ok, we can add a new corner
                zone->AppendCorner( GetScreen()->GetCrossHairPosition() );
                SetCurItem( zone );     // calls DisplayInfo().
            }
        }
    }

    return zone->GetNumCorners();
}


bool PCB_EDIT_FRAME::End_Zone( wxDC* DC )
{
    ZONE_CONTAINER* zone = GetBoard()->m_CurrentZoneContour;

    if( !zone )
        return true;

    // Validate the current outline:
    if( zone->GetNumCorners() <= 2 )   // An outline must have 3 corners or more
    {
        Abort_Zone_Create_Outline( m_canvas, DC );
        return true;
    }

    // Validate the current edge:
    int icorner = zone->GetNumCorners() - 1;
    if( zone->IsOnCopperLayer() )
    {
        if( Drc_On && m_drc->Drc( zone, icorner - 1 ) == BAD_DRC )  // we can't validate last edge
            return false;

        if( Drc_On && m_drc->Drc( zone, icorner ) == BAD_DRC )      // we can't validate the closing edge
        {
            DisplayError( this,
                          _( "DRC error: closing this area creates a drc error with an other area" ) );
            m_canvas->MoveCursorToCrossHair();
            return false;
        }
    }

    zone->ClearFlags();

    zone->DrawWhileCreateOutline( m_canvas, DC, GR_XOR );

    m_canvas->SetMouseCapture( NULL, NULL );

    // Undraw old drawings, because they can have important changes
    int layer = zone->GetLayer();
    GetBoard()->RedrawAreasOutlines( m_canvas, DC, GR_XOR, layer );
    GetBoard()->RedrawFilledAreas( m_canvas, DC, GR_XOR, layer );

    // Save initial zones configuration, for undo/redo, before adding new zone
    _AuxiliaryList.ClearListAndDeleteItems();
    s_PickedList.ClearListAndDeleteItems();
    SaveCopyOfZones(s_PickedList, GetBoard(), zone->GetNet(), zone->GetLayer() );

    // Put new zone in list
    if( !s_CurrentZone )
    {
        zone->m_Poly->Close(); // Close the current corner list
        GetBoard()->Add( zone );
        GetBoard()->m_CurrentZoneContour = NULL;

        // Add this zone in picked list, as new item
        ITEM_PICKER picker( zone, UR_NEW );
        s_PickedList.PushItem( picker );
    }
    else    // Append this outline as a cutout to an existing zone
    {
        for( int ii = 0; ii < zone->GetNumCorners(); ii++ )
        {
            s_CurrentZone->AppendCorner( zone->GetCornerPosition( ii ) );
        }

        s_CurrentZone->m_Poly->Close(); // Close the current corner list
        zone->RemoveAllContours();      // All corners are copied in s_CurrentZone. Free corner list.
        zone = s_CurrentZone;
    }

    s_AddCutoutToCurrentZone = false;
    s_CurrentZone = NULL;

    GetScreen()->SetCurItem( NULL );       // This outline can be deleted when merging outlines

    // Combine zones if possible :
    GetBoard()->AreaPolygonModified( &_AuxiliaryList, zone, true, s_Verbose );

    // Redraw the real edge zone :
    GetBoard()->RedrawAreasOutlines( m_canvas, DC, GR_OR, layer );
    GetBoard()->RedrawFilledAreas( m_canvas, DC, GR_OR, layer );

    int ii = GetBoard()->GetAreaIndex( zone );   // test if zone exists

    if( ii < 0 )
        zone = NULL;                        // was removed by combining zones

    int error_count = GetBoard()->Test_Drc_Areas_Outlines_To_Areas_Outlines( zone, true );

    if( error_count )
    {
        DisplayError( this, _( "Area: DRC outline error" ) );
    }

    UpdateCopyOfZonesList( s_PickedList, _AuxiliaryList, GetBoard() );
    SaveCopyInUndoList(s_PickedList, UR_UNSPECIFIED);
    s_PickedList.ClearItemsList(); // s_ItemsListPicker is no more owner of picked items

    OnModify();
    return true;
}


/* Redraws the zone outlines when moving mouse
 */
static void Show_New_Edge_While_Move_Mouse( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
                                            const wxPoint& aPosition, bool aErase )
{
    PCB_EDIT_FRAME* pcbframe = (PCB_EDIT_FRAME*) aPanel->GetParent();
    wxPoint         c_pos    = pcbframe->GetScreen()->GetCrossHairPosition();
    ZONE_CONTAINER* zone = pcbframe->GetBoard()->m_CurrentZoneContour;

    if( !zone )
        return;

    int icorner = zone->GetNumCorners() - 1;

    if( icorner < 1 )
        return;     // We must have 2 (or more) corners

    if( aErase )    // Undraw edge in old position
    {
        zone->DrawWhileCreateOutline( aPanel, aDC );
    }

    // Redraw the current edge in its new position
    if( pcbframe->GetZoneSettings().m_Zone_45_Only )
    {
        // calculate the new position as allowed
        wxPoint StartPoint = zone->GetCornerPosition( icorner - 1 );
        CalculateSegmentEndPoint( c_pos, StartPoint.x, StartPoint.y, &c_pos.x, &c_pos.y );
    }

    zone->SetCornerPosition( icorner, c_pos );

    zone->DrawWhileCreateOutline( aPanel, aDC );
}


void PCB_EDIT_FRAME::Edit_Zone_Params( wxDC* DC, ZONE_CONTAINER* aZone )
{
    ZONE_EDIT_T     edited;
    ZONE_SETTINGS   zoneInfo = GetZoneSettings();

    m_canvas->SetIgnoreMouseEvents( true );

    // Save initial zones configuration, for undo/redo, before adding new zone
    // note the net name and the layer can be changed, so we must save all zones
    _AuxiliaryList.ClearListAndDeleteItems();
    s_PickedList.ClearListAndDeleteItems();
    SaveCopyOfZones(s_PickedList, GetBoard(), -1, -1 );

    if( aZone->GetLayer() < FIRST_NO_COPPER_LAYER )
    {
        // edit a zone on a copper layer

        zoneInfo << *aZone;

        edited = InvokeCopperZonesEditor( this, &zoneInfo );
    }
    else
    {
        edited = InvokeNonCopperZonesEditor( this, aZone, &zoneInfo );
    }

    m_canvas->MoveCursorToCrossHair();
    m_canvas->SetIgnoreMouseEvents( false );

    if( edited == ZONE_ABORT )
    {
        _AuxiliaryList.ClearListAndDeleteItems();
        s_PickedList.ClearListAndDeleteItems();
        return;
    }

    SetZoneSettings( zoneInfo );

    if( edited == ZONE_EXPORT_VALUES )
    {
        UpdateCopyOfZonesList( s_PickedList, _AuxiliaryList, GetBoard() );
        SaveCopyInUndoList(s_PickedList, UR_UNSPECIFIED);
        s_PickedList.ClearItemsList(); // s_ItemsListPicker is no more owner of picked items
        return;
    }

    // Undraw old zone outlines
    for( int ii = 0; ii < GetBoard()->GetAreaCount(); ii++ )
    {
        ZONE_CONTAINER* edge_zone = GetBoard()->GetArea( ii );
        edge_zone->Draw( m_canvas, DC, GR_XOR );
    }

    zoneInfo.ExportSetting( *aZone );

    NETINFO_ITEM* net = GetBoard()->FindNet( zoneInfo.m_NetcodeSelection );

    if( net )   // net == NULL should not occur
        aZone->m_Netname = net->GetNetname();

    // Combine zones if possible
    GetBoard()->AreaPolygonModified( &_AuxiliaryList, aZone, true, s_Verbose );

    // Redraw the real new zone outlines
    GetBoard()->RedrawAreasOutlines( m_canvas, DC, GR_OR, -1 );

    UpdateCopyOfZonesList( s_PickedList, _AuxiliaryList, GetBoard() );
    SaveCopyInUndoList(s_PickedList, UR_UNSPECIFIED);

    s_PickedList.ClearItemsList();  // s_ItemsListPicker is no longer owner of picked items

    OnModify();
}


void PCB_EDIT_FRAME::Delete_Zone_Contour( wxDC* DC, ZONE_CONTAINER* aZone )
{
    int      ncont = aZone->m_Poly->GetContour( aZone->m_CornerSelection );

    EDA_RECT dirty = aZone->GetBoundingBox();

    // For compatibility with old boards: remove old SEGZONE fill segments
    Delete_OldZone_Fill( NULL, aZone->GetTimeStamp() );

    // Remove current filling:
    aZone->UnFill();

    if( ncont == 0 )    // This is the main outline: remove all
    {
        SaveCopyInUndoList( aZone, UR_DELETED );
        GetBoard()->Remove( aZone );
    }

    else
    {
        aZone->m_Poly->RemoveContour( ncont );
    }

    m_canvas->RefreshDrawingRect( dirty );

    OnModify();
}
