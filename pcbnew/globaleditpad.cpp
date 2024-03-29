/**
 * @file globaleditpad.cpp
 */

#include <fctsys.h>
#include <common.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <wxPcbStruct.h>
#include <pcbcommon.h>
#include <module_editor_frame.h>

#include <class_board.h>
#include <class_module.h>

#include <pcbnew.h>
#include <dialog_global_pads_edition_base.h>


/************************************/
/* class DIALOG_GLOBAL_PADS_EDITION */
/************************************/

class DIALOG_GLOBAL_PADS_EDITION : public DIALOG_GLOBAL_PADS_EDITION_BASE
{
private:
    PCB_BASE_FRAME* m_Parent;
    D_PAD*      m_CurrentPad;

public:
    static bool m_Pad_Shape_Filter;
    static bool m_Pad_Layer_Filter;
    static bool m_Pad_Orient_Filter;

public:
    DIALOG_GLOBAL_PADS_EDITION( PCB_BASE_FRAME* aParent, D_PAD* aPad );

private:
    void InstallPadEditor( wxCommandEvent& event );
    void PadPropertiesAccept( wxCommandEvent& event );
    void OnCancelClick( wxCommandEvent& event );
};


DIALOG_GLOBAL_PADS_EDITION::DIALOG_GLOBAL_PADS_EDITION( PCB_BASE_FRAME* aParent, D_PAD* aPad ) :
    DIALOG_GLOBAL_PADS_EDITION_BASE( aParent )
{
    m_Parent     = aParent;
    m_CurrentPad = aPad;

    // Pad filter selection.
    m_Pad_Shape_Filter_CB->SetValue( m_Pad_Shape_Filter );
    m_Pad_Layer_Filter_CB->SetValue( m_Pad_Layer_Filter );
    m_Pad_Orient_Filter_CB->SetValue( m_Pad_Orient_Filter );

    SetFocus();

    GetSizer()->Fit( this );
    Centre();
}


// Class DIALOG_GLOBAL_PADS_EDITION static variables
bool DIALOG_GLOBAL_PADS_EDITION::m_Pad_Shape_Filter  = true;
bool DIALOG_GLOBAL_PADS_EDITION::m_Pad_Layer_Filter  = true;
bool DIALOG_GLOBAL_PADS_EDITION::m_Pad_Orient_Filter = true;


void DIALOG_GLOBAL_PADS_EDITION::OnCancelClick( wxCommandEvent& event )
{
    EndModal( -1 );
}


/* Calls the Pad editor.
 */
void DIALOG_GLOBAL_PADS_EDITION::InstallPadEditor( wxCommandEvent& event )
{
    m_Parent->InstallPadOptionsFrame( m_CurrentPad );
}


/* Update the parameters for the component being edited.
 */
void DIALOG_GLOBAL_PADS_EDITION::PadPropertiesAccept( wxCommandEvent& event )
{
    int returncode = 0;

    switch( event.GetId() )
    {
    case ID_CHANGE_ID_MODULES:
        returncode = 1;

    // Fall through

    case ID_CHANGE_CURRENT_MODULE:
        m_Pad_Shape_Filter  = m_Pad_Shape_Filter_CB->GetValue();
        m_Pad_Layer_Filter  = m_Pad_Layer_Filter_CB->GetValue();
        m_Pad_Orient_Filter = m_Pad_Orient_Filter_CB->GetValue();
        EndModal( returncode );
        break;
    }

    m_Parent->OnModify();
}


/*
 * PCB_EDIT_FRAME::Function DlgGlobalChange_PadSettings
 * Function to change pad caracteristics for the given footprint
 * or alls footprints which look like the given footprint
 * Options are set by the opened dialog.
 * aPad is the pattern. The given footprint is the parent of this pad
 * aRedraw: if true: redraws the footprint
 */
void PCB_EDIT_FRAME::DlgGlobalChange_PadSettings( D_PAD* aPad, bool aRedraw )
{
    int     diag;

    if( aPad == NULL )
        aPad = &GetDesignSettings().m_Pad_Master;

    MODULE* module = aPad->GetParent();

    if( module == NULL )
    {
        DisplayError( this, wxT( "Global_Import_Pad_Settings() Error: NULL module" ) );
        return;
    }

    module->DisplayInfo( this );

    {
        DIALOG_GLOBAL_PADS_EDITION dlg( this, aPad );

        diag = dlg.ShowModal();
    }

    if( diag == -1 )
        return;

    bool edit_Same_Modules = false;
    if( diag == 1 )
        edit_Same_Modules = true;

    GlobalChange_PadSettings( aPad,edit_Same_Modules,
                              DIALOG_GLOBAL_PADS_EDITION::m_Pad_Shape_Filter,
                              DIALOG_GLOBAL_PADS_EDITION::m_Pad_Orient_Filter,
                              DIALOG_GLOBAL_PADS_EDITION::m_Pad_Layer_Filter,
                              aRedraw, true );
}

/*
 * FOOTPRINT_EDIT_FRAME::Function DlgGlobalChange_PadSettings
 * Function to change pad caracteristics for the given footprint
 * or alls footprints which look like the given footprint
 * Options are set by the opened dialog.
 * aPad is the pattern. The given footprint is the parent of this pad
 */
void FOOTPRINT_EDIT_FRAME::DlgGlobalChange_PadSettings( D_PAD* aPad )
{
    int     diag;

    if( aPad == NULL )
        aPad = &GetDesignSettings().m_Pad_Master;

    MODULE* module = aPad->GetParent();

    if( module == NULL )
    {
        DisplayError( this, wxT( "Global_Import_Pad_Settings() Error: NULL module" ) );
        return;
    }

    module->DisplayInfo( this );

    {
        DIALOG_GLOBAL_PADS_EDITION dlg( this, aPad );

        dlg.m_buttonIdModules->Enable( false );

        diag = dlg.ShowModal();
    }

    if( diag == -1 )
        return;

    bool edit_Same_Modules = false;
    if( diag == 1 )
        edit_Same_Modules = true;

    GlobalChange_PadSettings( aPad, edit_Same_Modules,
                              DIALOG_GLOBAL_PADS_EDITION::m_Pad_Shape_Filter,
                              DIALOG_GLOBAL_PADS_EDITION::m_Pad_Orient_Filter,
                              DIALOG_GLOBAL_PADS_EDITION::m_Pad_Layer_Filter,
                              true, false );
}

/*
 * Function GlobalChange_PadSettings
 * Function to change pad caracteristics for the given footprint
 * or alls footprints which look like the given footprint
 * aPad is the pattern. The given footprint is the parent of this pad
 * aSameFootprints: if true, make changes on all identical footprints
 * aPadShapeFilter: if true, make changes only on pads having the same shape as aPad
 * aPadOrientFilter: if true, make changes only on pads having the same orientation as aPad
 * aPadLayerFilter: if true, make changes only on pads having the same layers as aPad
 * aRedraw: if true: redraws the footprint
 * aSaveForUndo: if true: create an entry in the Undo/Redo list
 *        (usually: true in Schematic editor, false in Module editor)
 */
void PCB_BASE_FRAME::GlobalChange_PadSettings( D_PAD* aPad,
                                               bool aSameFootprints,
                                               bool aPadShapeFilter,
                                               bool aPadOrientFilter,
                                               bool aPadLayerFilter,
                                               bool aRedraw, bool aSaveForUndo )
{
    if( aPad == NULL )
        aPad = &GetDesignSettings().m_Pad_Master;

    MODULE* module = aPad->GetParent();

    if( module == NULL )
    {
        DisplayError( this, wxT( "Global_Import_Pad_Settings() Error: NULL module" ) );
        return;
    }

    // Search and copy the name of library reference.
    MODULE* Module_Ref = module;
    int pad_orient = aPad->GetOrientation() - Module_Ref->GetOrientation();

    // Prepare an undo list:
    if( aSaveForUndo )
    {
        PICKED_ITEMS_LIST itemsList;

        for( module = m_Pcb->m_Modules;  module;  module = module->Next() )
        {
            if( !aSameFootprints && (module != Module_Ref) )
                continue;

            if( module->m_LibRef != Module_Ref->m_LibRef )
                continue;

            bool   saveMe = false;

            for( D_PAD* pad = module->m_Pads;  pad;  pad = pad->Next() )
            {
                // Filters changes prohibited.
                if( aPadShapeFilter && ( pad->GetShape() != aPad->GetShape() ) )
                    continue;

                int currpad_orient = pad->GetOrientation() - module->GetOrientation();

                if( aPadOrientFilter && ( currpad_orient != pad_orient ) )
                    continue;

                if( aPadLayerFilter  &&  pad->GetLayerMask() != aPad->GetLayerMask() )
                    continue;

                saveMe = true;
            }

            if( saveMe )
            {
                ITEM_PICKER itemWrapper( module, UR_CHANGED );

                itemsList.PushItem( itemWrapper );
            }
        }

        SaveCopyInUndoList( itemsList, UR_CHANGED );
    }

    // Update the current module and same others modules if requested.
    for( module = m_Pcb->m_Modules;  module;  module = module->Next() )
    {
        if( !aSameFootprints && (module != Module_Ref) )
            continue;

        if( module->m_LibRef != Module_Ref->m_LibRef )
            continue;

        // Erase module on screen
        if( aRedraw )
        {
            module->SetFlags( DO_NOT_DRAW );
            m_canvas->RefreshDrawingRect( module->GetBoundingBox() );
            module->ClearFlags( DO_NOT_DRAW );
        }

        for( D_PAD* pad = module->m_Pads;  pad;  pad = pad->Next() )
        {
            // Filters changes prohibited.
            if( aPadShapeFilter && ( pad->GetShape() != aPad->GetShape() ) )
                continue;

            if( aPadOrientFilter &&  (pad->GetOrientation() - module->GetOrientation()) != pad_orient )
                continue;

            if( aPadLayerFilter )
            {
                if( pad->GetLayerMask() != aPad->GetLayerMask() )
                    continue;
                else
                    m_Pcb->m_Status_Pcb &= ~( LISTE_RATSNEST_ITEM_OK | CONNEXION_OK);
            }

            // Change characteristics:
            pad->SetAttribute( aPad->GetAttribute() );
            pad->SetShape( aPad->GetShape() );

            pad->SetLayerMask( aPad->GetLayerMask() );

            pad->SetSize( aPad->GetSize() );
            pad->SetDelta( aPad->GetDelta() );
            pad->SetOffset( aPad->GetOffset() );

            pad->SetDrillSize( aPad->GetDrillSize() );
            pad->SetDrillShape( aPad->GetDrillShape() );

            pad->SetOrientation( pad_orient + module->GetOrientation() );

            // copy also local mask margins, because these parameters usually depend on
            // pad sizes and layers
            pad->SetLocalSolderMaskMargin( aPad->GetLocalSolderMaskMargin() );
            pad->SetLocalSolderPasteMargin( aPad->GetLocalSolderPasteMargin() );
            pad->SetLocalSolderPasteMarginRatio( aPad->GetLocalSolderPasteMarginRatio() );

            if( pad->GetShape() != PAD_TRAPEZOID )
            {
                pad->SetDelta( wxSize( 0, 0 ) );
            }

            if( pad->GetShape() == PAD_CIRCLE )
            {
                // Ensure pad size.y = pad size.x
                int size = pad->GetSize().x;
                pad->SetSize( wxSize( size, size ) );
            }

            switch( pad->GetAttribute() )
            {
            case PAD_SMD:
            case PAD_CONN:
                pad->SetDrillSize( wxSize( 0, 0 ) );
                pad->SetOffset( wxPoint( 0, 0 ) );
                break;

            default:
                break;
            }
        }

        module->CalculateBoundingBox();

        if( aRedraw )
            m_canvas->RefreshDrawingRect( module->GetBoundingBox() );
    }

    OnModify();
}
