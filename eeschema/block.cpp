/****************************************************/
/*	BLOCK.CPP										*/
/* Gestion des Operations sur Blocks et Effacements */
/****************************************************/

#include "fctsys.h"
#include "appl_wxstruct.h"
#include "gr_basic.h"
#include "common.h"
#include "class_drawpanel.h"
#include "confirm.h"
#include "block_commande.h"

#include "program.h"
#include "general.h"
#include "class_marker_sch.h"
#include "class_library.h"
#include "protos.h"


/* Variables Locales */

// Imported functions:
void               MoveItemsInList( PICKED_ITEMS_LIST& aItemsList,
                                    const wxPoint      aMoveVector );
void               MirrorListOfItems( PICKED_ITEMS_LIST& aItemsList,
                                      wxPoint&           Center );
void               DeleteItemsInList( WinEDA_DrawPanel*  panel,
                                      PICKED_ITEMS_LIST& aItemsList );
void               DuplicateItemsInList( SCH_SCREEN*        screen,
                                         PICKED_ITEMS_LIST& aItemsList,
                                         const wxPoint      aMoveVector  );

/* Fonctions exportees */

/* Fonctions Locales */
static void           CollectStructsToDrag( SCH_SCREEN* screen );
static void           AddPickedItem( SCH_SCREEN* screen, wxPoint aPosition );
static LIB_DRAW_ITEM* GetNextPinPosition( SCH_COMPONENT* aDrawLibItem,
                                          wxPoint&       aPosition );
static void           DrawMovingBlockOutlines( WinEDA_DrawPanel* panel,
                                               wxDC*             DC,
                                               bool              erase );
static void           SaveStructListForPaste( PICKED_ITEMS_LIST& aItemsList );


/*************************************************************************/
int WinEDA_SchematicFrame::ReturnBlockCommand( int key )
/*************************************************************************/

/* Return the block command (BLOCK_MOVE, BLOCK_COPY...) corresponding to
 *  the key (ALT, SHIFT ALT ..)
 */
{
    int cmd;

    switch( key )
    {
    default:
        cmd = key & 0xFF;
        break;

    case 0:
        cmd = BLOCK_MOVE;
        break;

    case GR_KB_ALT:
    case GR_KB_SHIFT:
        cmd = BLOCK_COPY;
        break;

    case GR_KB_CTRL:
        cmd = BLOCK_DRAG;
        break;

    case GR_KB_SHIFTCTRL:
        cmd = BLOCK_DELETE;
        break;

    case MOUSE_MIDDLE:
        cmd = BLOCK_ZOOM;
        break;
    }

    return cmd;
}


/*************************************************/
void WinEDA_SchematicFrame::InitBlockPasteInfos()
/*************************************************/

/* Init the parameters used by the block paste command
 */
{
    BLOCK_SELECTOR* block = &GetScreen()->m_BlockLocate;

    block->m_ItemsSelection.CopyList( g_BlockSaveDataList.m_ItemsSelection );
    DrawPanel->ManageCurseur = DrawMovingBlockOutlines;
}


/******************************************************/
void WinEDA_SchematicFrame::HandleBlockPlace( wxDC* DC )
/******************************************************/

/* Routine to handle the BLOCK PLACE commande
 *  Last routine for block operation for:
 *  - block move & drag
 *  - block copie & paste
 */
{
    bool            err   = FALSE;
    BLOCK_SELECTOR* block = &GetScreen()->m_BlockLocate;

    if( DrawPanel->ManageCurseur == NULL )
    {
        err = TRUE;
        DisplayError( this, wxT( "HandleBlockPLace() : ManageCurseur = NULL" ) );
    }

    if( block->GetCount() == 0 )
    {
        wxString msg;
        err = TRUE;
        msg.Printf( wxT( "HandleBlockPLace() error : no items to place (cmd %d, state %d)" ),
                    block->m_Command, block->m_State );
        DisplayError( this, msg );
    }

    block->m_State = STATE_BLOCK_STOP;

    switch( block->m_Command )
    {
    case BLOCK_IDLE:
        err = TRUE;
        break;

    case BLOCK_DRAG:        /* Drag */
    case BLOCK_MOVE:        /* Move */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );

        SaveCopyInUndoList( block->m_ItemsSelection, UR_MOVED, block->m_MoveVector );

        MoveItemsInList( block->m_ItemsSelection, block->m_MoveVector );
        block->ClearItemsList();
        break;

    case BLOCK_COPY:                /* Copy */
    case BLOCK_PRESELECT_MOVE:      /* Move with preselection list*/
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );

        DuplicateItemsInList( GetScreen(), block->m_ItemsSelection, block->m_MoveVector );

        SaveCopyInUndoList(  block->m_ItemsSelection,
                             (block->m_Command == BLOCK_PRESELECT_MOVE) ? UR_CHANGED : UR_NEW );

        block->ClearItemsList();
        break;

    case BLOCK_PASTE:     /* Paste (recopie du dernier bloc sauve */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        PasteListOfItems( DC );
        block->ClearItemsList();
        break;

    case BLOCK_ZOOM:        // Handled by HandleBlockEnd()
    case BLOCK_DELETE:
    case BLOCK_SAVE:
    case BLOCK_ROTATE:
    case BLOCK_MIRROR_X:
    case BLOCK_MIRROR_Y:
    case BLOCK_FLIP:
    case BLOCK_ABORT:
    case BLOCK_SELECT_ITEMS_ONLY:
        break;
    }

    GetScreen()->SetModify();

    /* clear struct.m_Flags  */
    SCH_ITEM* Struct;
    for( Struct = GetScreen()->EEDrawList; Struct != NULL; Struct = Struct->Next() )
        Struct->m_Flags = 0;

    DrawPanel->ManageCurseur = NULL;
    DrawPanel->ForceCloseManageCurseur = NULL;
    block->m_Flags   = 0;
    block->m_State   = STATE_NO_BLOCK;
    block->m_Command = BLOCK_IDLE;
    GetScreen()->SetCurItem( NULL );

    TestDanglingEnds( GetScreen()->EEDrawList, DC );

    if( block->GetCount() )
    {
        DisplayError( this, wxT( "HandleBlockPLace() error: some items left in buffer" ) );
        block->ClearItemsList();
    }

    SetToolID( m_ID_current_state, DrawPanel->m_PanelDefaultCursor,
               wxEmptyString );
    DrawPanel->Refresh( );
}


/****************************************************/
int WinEDA_SchematicFrame::HandleBlockEnd( wxDC* DC )
/****************************************************/

/* Routine de gestion de la commande BLOCK END
 *  retourne :
 *  0 si aucun composant selectionne
 *  1 sinon
 *  -1 si commande terminee et composants trouves (block delete, block save)
 */
{
    int             ii = 0;
    bool            zoom_command = FALSE;
    BLOCK_SELECTOR* block = &GetScreen()->m_BlockLocate;

    if( block->GetCount() )
    {
        BlockState   state   = block->m_State;
        CmdBlockType command = block->m_Command;
        if( DrawPanel->ForceCloseManageCurseur )
            DrawPanel->ForceCloseManageCurseur( DrawPanel, DC );
        block->m_State   = state;
        block->m_Command = command;
        DrawPanel->ManageCurseur = DrawAndSizingBlockOutlines;
        DrawPanel->ForceCloseManageCurseur = AbortBlockCurrentCommand;
        GetScreen()->m_Curseur = block->GetEnd();
        if( block->m_Command != BLOCK_ABORT )
            DrawPanel->MouseToCursorSchema();
    }

    if( DrawPanel->ManageCurseur != NULL )
        switch( block->m_Command )
        {
        case BLOCK_IDLE:
            DisplayError( this, wxT( "Error in HandleBlockPLace()" ) );
            break;

        case BLOCK_DRAG: /* Drag */
            BreakSegmentOnJunction( (SCH_SCREEN*) GetScreen() );
        case BLOCK_MOVE:    /* Move */
        case BLOCK_COPY:    /* Copy */
            PickItemsInBlock( GetScreen()->m_BlockLocate, GetScreen() );

        case BLOCK_PRESELECT_MOVE: /* Move with preselection list*/
            if( block->GetCount() )
            {
                ii = 1;
                CollectStructsToDrag( GetScreen() );
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
                DrawPanel->ManageCurseur = DrawMovingBlockOutlines;
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
                block->m_State = STATE_BLOCK_MOVE;
            }
            else
            {
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
                DrawPanel->ManageCurseur = NULL;
                DrawPanel->ForceCloseManageCurseur = NULL;
            }
            break;

        case BLOCK_DELETE: /* Delete */
            PickItemsInBlock( GetScreen()->m_BlockLocate, GetScreen() );
            DrawAndSizingBlockOutlines( DrawPanel, DC, FALSE );
            if( block->GetCount() )
            {
                ii = -1;
                DeleteItemsInList( DrawPanel, block->m_ItemsSelection );
                GetScreen()->SetModify();
            }
            block->ClearItemsList();
            TestDanglingEnds( GetScreen()->EEDrawList, DC );
            DrawPanel->Refresh();
            break;

        case BLOCK_SAVE: /* Save */
            PickItemsInBlock( GetScreen()->m_BlockLocate, GetScreen() );
            DrawAndSizingBlockOutlines( DrawPanel, DC, FALSE );
            if( block->GetCount() )
            {
                wxPoint move_vector =  -GetScreen()->m_BlockLocate.m_BlockLastCursorPosition;
                SaveStructListForPaste( block->m_ItemsSelection );
                MoveItemsInList( g_BlockSaveDataList.m_ItemsSelection, move_vector);
                ii = -1;
            }
            block->ClearItemsList();
            break;

        case BLOCK_PASTE:
            block->m_State = STATE_BLOCK_MOVE;
            break;

        case BLOCK_FLIP: /* pcbnew only! */
            break;

        case BLOCK_ROTATE:
        case BLOCK_MIRROR_X:
        case BLOCK_MIRROR_Y:
            break;

        case BLOCK_ZOOM: /* Window Zoom */
            zoom_command = TRUE;
            break;

        case BLOCK_SELECT_ITEMS_ONLY:   /* Not used */
        case BLOCK_ABORT:               /* not executed here */
            break;
        }

    if( block->m_Command  == BLOCK_ABORT )
    {
        /* clear struct.m_Flags  */
        EDA_BaseStruct* Struct;
        for( Struct = GetScreen()->EEDrawList; Struct != NULL; Struct = Struct->Next() )
            Struct->m_Flags = 0;
    }

    if( ii <= 0 )
    {
        block->m_Flags   = 0;
        block->m_State   = STATE_NO_BLOCK;
        block->m_Command = BLOCK_IDLE;
        DrawPanel->ManageCurseur = NULL;
        DrawPanel->ForceCloseManageCurseur = NULL;
        GetScreen()->SetCurItem( NULL );
        SetToolID( m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
    }

    if( zoom_command )
        Window_Zoom( GetScreen()->m_BlockLocate );

    return ii;
}


/***********************************************************************/
void WinEDA_SchematicFrame::HandleBlockEndByPopUp( int Command, wxDC* DC )
/***********************************************************************/

/* Routine de gestion de la commande BLOCK END by PopUp
 *  Appelee apres HandleBlockEnd.
 *  A partir de la commande bloc move, peut executer une commande autre que bloc move.
 */
{
    int             ii    = 0;
    BLOCK_SELECTOR* block = &GetScreen()->m_BlockLocate;

    if( block->m_Command != BLOCK_MOVE )
        return;
    if( Command == BLOCK_MOVE )
        return;

    block->m_Command = (CmdBlockType) Command;
    block->SetMessageBlock( this );

    switch( block->m_Command )
    {
    case BLOCK_COPY:     /* move to copy */
        block->m_State = STATE_BLOCK_MOVE;
        ii = 1;
        break;

    case BLOCK_DRAG:     /* move to Drag */

        /* Effacement de la liste des structures de pointage,
         *  qui est devenue erronnee */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        block->ClearItemsList();

        BreakSegmentOnJunction( GetScreen() );

        PickItemsInBlock( GetScreen()->m_BlockLocate, GetScreen() );
        if( block->GetCount() )
        {
            ii = 1;
            CollectStructsToDrag( (SCH_SCREEN*) GetScreen() );
            if( DrawPanel->ManageCurseur )
                DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
            block->m_State = STATE_BLOCK_MOVE;
        }
        break;

    case BLOCK_DELETE:     /* move to Delete */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->GetCount() )
        {
            ii = -1;
            DeleteItemsInList( DrawPanel, block->m_ItemsSelection );
            GetScreen()->SetModify();
        }
        TestDanglingEnds( GetScreen()->EEDrawList, DC );
        DrawPanel->Refresh();
        DrawPanel->Refresh();
        break;

    case BLOCK_SAVE:     /* Save */
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->GetCount() )
        {
            wxPoint move_vector =  -GetScreen()->m_BlockLocate.m_BlockLastCursorPosition;
            SaveStructListForPaste( block->m_ItemsSelection );
            MoveItemsInList( g_BlockSaveDataList.m_ItemsSelection, move_vector );
            ii = -1;
        }
        break;

    case BLOCK_ZOOM:     /* Window Zoom */
        DrawPanel->ForceCloseManageCurseur( DrawPanel, DC );
        DrawPanel->SetCursor(
            DrawPanel->m_PanelCursor = DrawPanel->m_PanelDefaultCursor );
        Window_Zoom( GetScreen()->m_BlockLocate );
        break;


    case BLOCK_ROTATE:
        break;

    case BLOCK_MIRROR_X:
    case BLOCK_MIRROR_Y:
        if( DrawPanel->ManageCurseur )
            DrawPanel->ManageCurseur( DrawPanel, DC, FALSE );
        if( block->GetCount() )
        {
            ii = -1;
            /* Compute the mirror centre and put it on grid */
            wxPoint mirrorPoint = block->Centre();
            PutOnGrid( &mirrorPoint );
            SaveCopyInUndoList( block->m_ItemsSelection, UR_MIRRORED_Y, mirrorPoint );
            MirrorListOfItems( block->m_ItemsSelection, mirrorPoint );
            GetScreen()->SetModify();
        }
        TestDanglingEnds( GetScreen()->EEDrawList, DC );
        DrawPanel->Refresh();
        break;

    default:
        break;
    }

    if( ii <= 0 )
    {
        block->ClearItemsList();
        block->m_Flags   = 0;
        block->m_State   = STATE_NO_BLOCK;
        block->m_Command = BLOCK_IDLE;
        DrawPanel->ManageCurseur = NULL;
        DrawPanel->ForceCloseManageCurseur = NULL;
        GetScreen()->SetCurItem( NULL );
        SetToolID( m_ID_current_state, DrawPanel->m_PanelDefaultCursor, wxEmptyString );
    }
}


/************************************************************************/
static void DrawMovingBlockOutlines( WinEDA_DrawPanel* panel, wxDC* DC,
                                     bool erase )
/************************************************************************/

/* Retrace le contour du block de recherche de structures
 *  L'ensemble du block suit le curseur
 */
{
    BLOCK_SELECTOR* block = &panel->GetScreen()->m_BlockLocate;;

    BASE_SCREEN*    screen = panel->GetScreen();
    SCH_ITEM*       schitem;

    /* Effacement ancien cadre */
    if( erase )
    {
        block->Draw( panel, DC, block->m_MoveVector, g_XorMode, block->m_Color );
        for( unsigned ii = 0; ii < block->GetCount(); ii++ )
        {
            schitem = (SCH_ITEM*) block->m_ItemsSelection.GetPickedItem( ii );
            DrawStructsInGhost( panel, DC, schitem, block->m_MoveVector );
        }
    }

    /* Redessin nouvel affichage */

    block->m_MoveVector = screen->m_Curseur - block->m_BlockLastCursorPosition;

    block->Draw( panel, DC, block->m_MoveVector, g_XorMode, block->m_Color );

    for( unsigned ii = 0; ii < block->GetCount(); ii++ )
    {
        schitem = (SCH_ITEM*) block->m_ItemsSelection.GetPickedItem( ii );
        DrawStructsInGhost( panel, DC, schitem, block->m_MoveVector );
    }
}


/*****************************************************************/
void SaveStructListForPaste( PICKED_ITEMS_LIST& aItemsList )
/*****************************************************************/

/* Routine to Save an object from global drawing object list.
 *  This routine is the same as delete but:
 *  - the original list is NOT removed.
 *  - List is saved in g_BlockSaveDataList
 */
{
    g_BlockSaveDataList.ClearListAndDeleteItems();      // delete previous saved list, if exists

    /* save the new list: */
    ITEM_PICKER item;
    for( unsigned ii = 0; ii < aItemsList.GetCount(); ii++ )
    {
        /* Make a copy of the original picked item. */
        SCH_ITEM* DrawStructCopy = DuplicateStruct( (SCH_ITEM*) aItemsList.GetPickedItem( ii ) );
        DrawStructCopy->SetParent( NULL );
        item.m_PickedItem = DrawStructCopy;
        g_BlockSaveDataList.PushItem( item );
    }
}


/*****************************************************************************
* Routine to paste a structure from the g_BlockSaveDataList stack.
*	This routine is the same as undelete but original list is NOT removed.
*****************************************************************************/
void WinEDA_SchematicFrame::PasteListOfItems( wxDC* DC )
{
    SCH_ITEM* Struct;

    if( g_BlockSaveDataList.GetCount() == 0 )
    {
        DisplayError( this, wxT( "No struct to paste" ) );
        return;
    }

    PICKED_ITEMS_LIST picklist;

    // Creates data, and push it as new data in undo item list buffer
    ITEM_PICKER picker( NULL, UR_NEW );
    for( unsigned ii = 0; ii < g_BlockSaveDataList.GetCount(); ii++ )
    {
        Struct = DuplicateStruct( (SCH_ITEM*) g_BlockSaveDataList.m_ItemsSelection.GetPickedItem( ii ) );
        picker.m_PickedItem = Struct;
        picklist.PushItem( picker );

        // Clear annotation and init new time stamp for the new components:
        if( Struct->Type() == TYPE_SCH_COMPONENT )
        {
            ( (SCH_COMPONENT*) Struct )->m_TimeStamp = GetTimeStamp();
            ( (SCH_COMPONENT*) Struct )->ClearAnnotation( NULL );
        }
        SetaParent( Struct, GetScreen() );
        RedrawOneStruct( DrawPanel, DC, Struct, GR_DEFAULT_DRAWMODE );
        Struct->SetNext( GetScreen()->EEDrawList );
        GetScreen()->EEDrawList = Struct;
    }

    SaveCopyInUndoList( picklist, UR_NEW );

    MoveItemsInList( picklist, GetScreen()->m_BlockLocate.m_MoveVector );

    /* clear .m_Flags member for all items */
    for( Struct = GetScreen()->EEDrawList; Struct != NULL; Struct = Struct->Next() )
        Struct->m_Flags = 0;

    GetScreen()->SetModify();

    return;
}


/****************************************************/
static void CollectStructsToDrag( SCH_SCREEN* screen )
/****************************************************/

/* creates the list of items found when a drag block is initiated.
 * items are those selected in window block an some items outside this area but connected
 * to a selected item (connected wires to a component or an entry )
 */
{
    SCH_ITEM*           Struct;
    EDA_DrawLineStruct* SegmStruct;

    PICKED_ITEMS_LIST*  pickedlist = &screen->m_BlockLocate.m_ItemsSelection;

    if( pickedlist->GetCount() == 0 )
        return;

    /* .m_Flags member is used to handle how a wire is exactly selected
     * (fully selected, or partially selected by an end point )
     */
    for( Struct = screen->EEDrawList; Struct != NULL; Struct = Struct->Next() )
        Struct->m_Flags = 0;

    // Sel .m_Flags to selected for a wire or bus in selected area if there is only one item:
    if( pickedlist->GetCount() == 1 )
    {
        Struct = (SCH_ITEM*) pickedlist->GetPickedItem( 0 );
        if( Struct->Type() == DRAW_SEGMENT_STRUCT_TYPE )
            Struct->m_Flags = SELECTED;
    }
    // Sel .m_Flags to selected for a wire or bus in selected area for a list of items:
    else
    {
        for( unsigned ii = 0; ii < pickedlist->GetCount(); ii++ )
        {
            Struct = (SCH_ITEM*) pickedlist->GetPickedItem( ii );
            Struct->m_Flags = SELECTED;
        }
    }

    if( screen->m_BlockLocate.m_Command != BLOCK_DRAG )
        return;


    /* Suppression du deplacement des extremites de segments hors cadre
     *  de selection */
    for( unsigned ii = 0; ii < pickedlist->GetCount(); ii++ )
    {
        Struct = (SCH_ITEM*)(SCH_ITEM*) pickedlist->GetPickedItem( ii );
        if( Struct->Type() == DRAW_SEGMENT_STRUCT_TYPE )
        {
            SegmStruct = (EDA_DrawLineStruct*) Struct;
            if( ! screen->m_BlockLocate.Inside(SegmStruct->m_Start) )
                SegmStruct->m_Flags |= STARTPOINT;

            if( ! screen->m_BlockLocate.Inside(SegmStruct->m_End) )
                SegmStruct->m_Flags |= ENDPOINT;

            // Save m_Flags for Undo/redo drag operations:
            pickedlist->SetPickerFlags(SegmStruct->m_Flags, ii);

        }
    }

    /* Search for other items to drag. They are end wires connected to selected items
     */

    for( unsigned ii = 0; ii < pickedlist->GetCount(); ii++ )
    {
        Struct = (SCH_ITEM*)(SCH_ITEM*) pickedlist->GetPickedItem( ii );
        if( Struct->Type() == TYPE_SCH_COMPONENT )
        {
            // Add all pins of the selected component to list
            LIB_DRAW_ITEM* DrawItem;
            wxPoint            pos;
            DrawItem = GetNextPinPosition( (SCH_COMPONENT*) Struct, pos );
            while( DrawItem )
            {
                if( ! screen->m_BlockLocate.Inside(pos) )
                {
                    // This pin is outside area,
                    // but because it it the pin of a selected component
                    // we must also select connected items to this pin
                    AddPickedItem( screen, pos );
                }

                DrawItem = GetNextPinPosition( NULL, pos );
            }
        }

        if( Struct->Type() == DRAW_SHEET_STRUCT_TYPE )
        {
            // Add all pins sheets of a selected hierarchical sheet to the list
            Hierarchical_PIN_Sheet_Struct* SLabel =
                ( (DrawSheetStruct*) Struct )->m_Label;
            while( SLabel )
            {
                if( SLabel->Type() == DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE )
                    AddPickedItem( screen, SLabel->m_Pos );
                SLabel = (Hierarchical_PIN_Sheet_Struct*) SLabel->Next();
            }
        }

        if( Struct->Type() == DRAW_BUSENTRY_STRUCT_TYPE )
        {
            DrawBusEntryStruct* item = (DrawBusEntryStruct*) Struct;
            AddPickedItem( screen, item->m_Pos );
            AddPickedItem( screen, item->m_End() );
        }
    }
}


/******************************************************************/
static void AddPickedItem( SCH_SCREEN* screen, wxPoint position )
/******************************************************************/

/** AddPickedItem
 * add to the picked list in screen->m_BlockLocate items found at location position
 * @param screen = the screen to consider
 * @param position = the wxPoint where items must be located to be select
 */
{
    SCH_ITEM*          Struct;

    /* Examen de la liste des elements deja selectionnes */
    PICKED_ITEMS_LIST* pickedlist = &screen->m_BlockLocate.m_ItemsSelection;

    for( unsigned ii = 0; ii < pickedlist->GetCount(); ii++ )
    {
        Struct = (SCH_ITEM*) pickedlist->GetPickedItem( ii );

        switch( Struct->Type() )
        {
        case DRAW_SEGMENT_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (EDA_DrawLineStruct*) Struct )
            if( STRUCT->m_Start == position )
                STRUCT->m_Flags &= ~STARTPOINT;

            if( STRUCT->m_End == position )
                STRUCT->m_Flags &= ~ENDPOINT;

            // Save m_Flags for Undo/redo drag operations:
            pickedlist->SetPickerFlags(STRUCT->m_Flags, ii);
            break;

        default:
            break;
        }
    }

    /* Examen de la liste des elements non selectionnes */

    ITEM_PICKER picker;
    Struct = screen->EEDrawList;
    while( Struct )
    {
        picker.m_PickedItem    = Struct;
        picker.m_PickedItemType = Struct->Type();
        switch( Struct->Type() )
        {
        case TYPE_NOT_INIT:
            break;

        case DRAW_POLYLINE_STRUCT_TYPE:
            if( Struct->m_Flags & SELECTED )
                break; /* Deja en liste */
            break;

        case DRAW_JUNCTION_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawJunctionStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break; /* Deja en liste */
            if( STRUCT->m_Pos != position )
                break;
            pickedlist->PushItem( picker );
            break;

        case DRAW_SEGMENT_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (EDA_DrawLineStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break; /* Deja en liste */
            if( STRUCT->m_Start == position )
            {
                Struct->m_Flags  = SELECTED | ENDPOINT | STARTPOINT;
                Struct->m_Flags &= ~STARTPOINT;
                // Save m_Flags for Undo/redo drag operations:
                picker.m_PickerFlags= Struct->m_Flags;
                pickedlist->PushItem( picker );
            }
            else if( STRUCT->m_End == position )
            {
                Struct->m_Flags  = SELECTED | ENDPOINT | STARTPOINT;
                Struct->m_Flags &= ~ENDPOINT;
                // Save m_Flags for Undo/redo drag operations:
                picker.m_PickerFlags= Struct->m_Flags;
                pickedlist->PushItem( picker );
            }
            break;

        case DRAW_BUSENTRY_STRUCT_TYPE:
            break;

        case TYPE_SCH_TEXT:
            break;

        case TYPE_SCH_LABEL:
                #undef STRUCT
                #define STRUCT ( (SCH_LABEL*) Struct )
            if( Struct->m_Flags & SELECTED )
                break; /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            Struct->m_Flags |= SELECTED;
            pickedlist->PushItem( picker );
            break;

        case TYPE_SCH_HIERLABEL:
        case TYPE_SCH_GLOBALLABEL:
                #undef STRUCT
                #define STRUCT ( (SCH_LABEL*) Struct )
            if( Struct->m_Flags & SELECTED )
                break; /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            Struct->m_Flags |= SELECTED;
            pickedlist->PushItem( picker );
            break;

        case TYPE_SCH_COMPONENT:
        case DRAW_SHEET_STRUCT_TYPE:
        case DRAW_HIERARCHICAL_PIN_SHEET_STRUCT_TYPE:
            break;

        case TYPE_MARKER_SCH:
                #undef STRUCT
                #define STRUCT ( (MARKER_SCH*) Struct )
            if( Struct->m_Flags & SELECTED )
                break; /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            Struct->m_Flags |= SELECTED;
            pickedlist->PushItem( picker );
            break;

        case DRAW_NOCONNECT_STRUCT_TYPE:
                #undef STRUCT
                #define STRUCT ( (DrawNoConnectStruct*) Struct )
            if( Struct->m_Flags & SELECTED )
                break; /* Already in list */
            if( STRUCT->m_Pos != position )
                break;
            Struct->m_Flags |= SELECTED;
            pickedlist->PushItem( picker );
            break;

        default:
            break;
        }
        Struct = Struct->Next();
    }
}


/*********************************************************************************/
static LIB_DRAW_ITEM* GetNextPinPosition( SCH_COMPONENT* aDrawLibItem,
                                          wxPoint&       aPosition )
/*********************************************************************************/

/** GetNextPinPosition()
 * calculate position of the "next" pin of the aDrawLibItem component
 * if aDrawLibItem is non null : search for the first pin
 * if aDrawLibItem == NULL, search the next pin
 * returns its position
 * @param aDrawLibItem = component test. search for the first pin
 *      if NULL, serach for the next pin for each call
 * @param aPosition = the calculated pin position, according to the component orientation and position
 * @return a pointer to the pin
 */
{
    LIB_COMPONENT* Entry;
    static LibDrawPin* NextPin;
    static int Multi, convert, TransMat[2][2];
    int orient;
    LibDrawPin* Pin;
    static wxPoint CmpPosition;

    if( aDrawLibItem )
    {
        NextPin = NULL;
        Entry = CMP_LIBRARY::FindLibraryComponent( aDrawLibItem->m_ChipName );

        if( Entry == NULL )
            return NULL;

        Pin         = Entry->GetNextPin();
        Multi       = aDrawLibItem->m_Multi;
        convert     = aDrawLibItem->m_Convert;
        CmpPosition = aDrawLibItem->m_Pos;
        memcpy( TransMat, aDrawLibItem->m_Transform, sizeof(TransMat) );
    }
    else
        Pin = NextPin;

    for( ; Pin != NULL; NextPin = Entry->GetNextPin( Pin ) )
    {
        wxASSERT( Pin->Type() == COMPONENT_PIN_DRAW_TYPE );

        /* Elimination des elements non relatifs a l'unite */
        if( Multi && Pin->m_Unit && ( Pin->m_Unit != Multi ) )
            continue;
        if( convert && Pin->m_Convert && ( Pin->m_Convert != convert ) )
            continue;

        /* Calcul de l'orientation reelle de la Pin */
        orient = Pin->ReturnPinDrawOrient( TransMat );

        /* Calcul de la position du point de reference */
        aPosition = TransformCoordinate( TransMat, Pin->m_Pos ) + CmpPosition;
        return Pin;
    }

    NextPin = NULL;
    return NULL;
}
