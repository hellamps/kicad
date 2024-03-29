/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2008-2011 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2004-2011 KiCad Developers, see change_log.txt for contributors.
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
 * @file eeschema/hotkeys.cpp
 */

#include <fctsys.h>
#include <eeschema_id.h>
#include <hotkeys.h>
#include <wxEeschemaStruct.h>

#include <general.h>
#include <protos.h>
#include <libeditframe.h>
#include <class_libentry.h>
#include <sch_junction.h>
#include <sch_line.h>
#include <sch_component.h>
#include <sch_sheet.h>

#include <dialogs/dialog_schematic_find.h>


/* How to add a new hotkey:
 * add a new id in the enum hotkey_id_command like MY_NEW_ID_FUNCTION (see
 * hotkeys.h).
 * add a new EDA_HOTKEY entry like:
 *  static EDA_HOTKEY HkMyNewEntry(wxT("Command Label"), MY_NEW_ID_FUNCTION,
 *                                    default key value);
 * wxT("Command Label") is the name used in hotkey list display, and the
 * identifier in the hotkey list file
 * MY_NEW_ID_FUNCTION is an equivalent id function used in the switch in
 * OnHotKey() function.
 * default key value is the default hotkey for this command. Can be overridden
 * by the user hotkey list file
 * add the HkMyNewEntry pointer in the s_Schematic_Hotkey_List list or the
 * s_LibEdit_Hotkey_List list or s_Common_Hotkey_List if the same command is
 * added both in Eeschema and libedit)
 * Add the new code in the switch in OnHotKey() function.
 * when the variable itemInEdit is true, an item is currently edited.
 * This can be useful if the new function cannot be executed while an item is
 * currently being edited
 * ( For example, one cannot start a new wire when a component is moving.)
 *
 * Note: If an hotkey is a special key be sure the corresponding wxWidget
 *       keycode (WXK_XXXX) is handled in the hotkey_name_descr
 *       s_Hotkey_Name_List list (see hotkeys_basic.cpp) and see this list
 *       for some ascii keys (space ...)
 *
 *  Key modifier are: GR_KB_CTRL GR_KB_ALT
 */


/* local variables */
/* Hotkey list: */

/**
 * Common commands
 */

/* Fit on Screen */
#if !defined( __WXMAC__ )
static EDA_HOTKEY HkZoomAuto( wxT( "Fit on Screen" ), HK_ZOOM_AUTO, WXK_HOME, ID_ZOOM_PAGE );
#else
static EDA_HOTKEY HkZoomAuto( wxT( "Zoom Auto" ), HK_ZOOM_AUTO, GR_KB_CTRL + '0',
                              ID_ZOOM_PAGE );
#endif

static EDA_HOTKEY HkZoomCenter( wxT( "Zoom Center" ), HK_ZOOM_CENTER, WXK_F4,
                                ID_POPUP_ZOOM_CENTER );

/* Refresh Screen */
#if !defined( __WXMAC__ )
static EDA_HOTKEY HkZoomRedraw( wxT( "Zoom Redraw" ), HK_ZOOM_REDRAW, WXK_F3, ID_ZOOM_REDRAW );
#else
static EDA_HOTKEY HkZoomRedraw( wxT( "Zoom Redraw" ), HK_ZOOM_REDRAW, GR_KB_CTRL + 'R',
                                ID_ZOOM_REDRAW );
#endif

/* Zoom In */
#if !defined( __WXMAC__ )
static EDA_HOTKEY HkZoomIn( wxT( "Zoom In" ), HK_ZOOM_IN, WXK_F1, ID_POPUP_ZOOM_IN );
#else
static EDA_HOTKEY HkZoomIn( wxT( "Zoom In" ), HK_ZOOM_IN, GR_KB_CTRL + '+', ID_POPUP_ZOOM_IN );
#endif

/* Zoom Out */
#if !defined( __WXMAC__ )
static EDA_HOTKEY HkZoomOut( wxT( "Zoom Out" ), HK_ZOOM_OUT, WXK_F2, ID_POPUP_ZOOM_OUT );
#else
static EDA_HOTKEY HkZoomOut( wxT( "Zoom Out" ), HK_ZOOM_OUT, GR_KB_CTRL + '-', ID_POPUP_ZOOM_OUT );
#endif

static EDA_HOTKEY HkHelp( wxT( "Help (this window)" ), HK_HELP, '?' );
static EDA_HOTKEY HkResetLocalCoord( wxT( "Reset Local Coordinates" ),
                                     HK_RESET_LOCAL_COORD, ' ' );

/* Undo */
static EDA_HOTKEY HkUndo( wxT( "Undo" ), HK_UNDO, GR_KB_CTRL + 'Z', (int) wxID_UNDO );

/* Redo */
#if !defined( __WXMAC__ )
static EDA_HOTKEY HkRedo( wxT( "Redo" ), HK_REDO, GR_KB_CTRL + 'Y', (int) wxID_REDO );
#else
static EDA_HOTKEY HkRedo( wxT( "Redo" ), HK_REDO, GR_KB_SHIFT + GR_KB_CTRL + 'Z',
                          (int) wxID_REDO );
#endif

// Schematic editor
static EDA_HOTKEY HkAddLabel( wxT( "Add Label" ), HK_ADD_LABEL, 'L', ID_LABEL_BUTT );
static EDA_HOTKEY HkAddHierarchicalLabel( wxT( "Add Hierarchical Label" ), HK_ADD_HLABEL, 'H',
                                          ID_HIERLABEL_BUTT );
static EDA_HOTKEY HkAddGlobalLabel( wxT( "Add Global Label" ), HK_ADD_GLABEL, GR_KB_CTRL + 'L',
                                    ID_GLABEL_BUTT );
static EDA_HOTKEY HkAddJunction( wxT( "Add Junction" ), HK_ADD_JUNCTION, 'J', ID_JUNCTION_BUTT );
static EDA_HOTKEY HkBeginWire( wxT( "Draw Wire" ), HK_BEGIN_WIRE, 'W', ID_WIRE_BUTT );
static EDA_HOTKEY HkBeginBus( wxT( "Draw Bus" ), HK_BEGIN_BUS, 'B', ID_BUS_BUTT );
static EDA_HOTKEY HkAddComponent( wxT( "Add Component" ), HK_ADD_NEW_COMPONENT, 'A',
                                  ID_SCH_PLACE_COMPONENT );
static EDA_HOTKEY HkAddPower( wxT( "Add Power" ), HK_ADD_NEW_POWER, 'P',
                              ID_PLACE_POWER_BUTT );
static EDA_HOTKEY HkAddNoConn( wxT( "Add NoConnected Flag" ), HK_ADD_NOCONN_FLAG, 'Q',
                               ID_NOCONN_BUTT );
static EDA_HOTKEY HkAddHierSheet( wxT( "Add Sheet" ), HK_ADD_HIER_SHEET, 'S',
                                  ID_SHEET_SYMBOL_BUTT );
static EDA_HOTKEY HkAddBusEntry( wxT( "Add Bus Entry" ), HK_ADD_BUS_ENTRY, '/',
                                 ID_BUSTOBUS_ENTRY_BUTT );
static EDA_HOTKEY HkAddWireEntry( wxT( "Add Wire Entry" ), HK_ADD_WIRE_ENTRY, 'Z',
                                  ID_WIRETOBUS_ENTRY_BUTT );
static EDA_HOTKEY HkAddGraphicPolyLine( wxT( "Add Graphic PolyLine" ), HK_ADD_GRAPHIC_POLYLINE,
                                        'I', ID_LINE_COMMENT_BUTT );
static EDA_HOTKEY HkAddGraphicText( wxT( "Add Graphic Text" ), HK_ADD_GRAPHIC_TEXT, 'T',
                                    ID_TEXT_COMMENT_BUTT );
static EDA_HOTKEY HkMirrorY( wxT( "Mirror Y Component" ), HK_MIRROR_Y_COMPONENT, 'Y',
                             ID_SCH_MIRROR_Y );
static EDA_HOTKEY HkMirrorX( wxT( "Mirror X Component" ), HK_MIRROR_X_COMPONENT, 'X',
                             ID_SCH_MIRROR_X );
static EDA_HOTKEY HkOrientNormalComponent( wxT( "Orient Normal Component" ),
                                           HK_ORIENT_NORMAL_COMPONENT, 'N', ID_SCH_ORIENT_NORMAL );
static EDA_HOTKEY HkRotate( wxT( "Rotate Item" ), HK_ROTATE, 'R', ID_SCH_ROTATE_CLOCKWISE );
static EDA_HOTKEY HkEdit( wxT( "Edit Schematic Item" ), HK_EDIT, 'E', ID_SCH_EDIT_ITEM );
static EDA_HOTKEY HkEditComponentValue( wxT( "Edit Component Value" ),
                                        HK_EDIT_COMPONENT_VALUE, 'V',
                                        ID_SCH_EDIT_COMPONENT_VALUE );
static EDA_HOTKEY HkEditComponentFootprint( wxT( "Edit Component Footprint" ),
                                            HK_EDIT_COMPONENT_FOOTPRINT, 'F',
                                            ID_SCH_EDIT_COMPONENT_FOOTPRINT );
static EDA_HOTKEY HkMove( wxT( "Move Schematic Item" ),
                          HK_MOVE_COMPONENT_OR_ITEM, 'M',
                          ID_SCH_MOVE_ITEM );

static EDA_HOTKEY HkCopyComponentOrText( wxT( "Copy Component or Label" ),
                                         HK_COPY_COMPONENT_OR_LABEL, 'C',
                                         ID_POPUP_SCH_COPY_ITEM );

static EDA_HOTKEY HkDrag( wxT( "Drag Schematic Item" ), HK_DRAG, 'G', ID_SCH_DRAG_ITEM );
static EDA_HOTKEY HkSaveBlock( wxT( "Save Block" ), HK_SAVE_BLOCK, 'C' + GR_KB_CTRL );
static EDA_HOTKEY HkMove2Drag( wxT( "Move Block -> Drag Block" ),
                               HK_MOVEBLOCK_TO_DRAGBLOCK, '\t' );
static EDA_HOTKEY HkInsert( wxT( "Repeat Last Item" ), HK_REPEAT_LAST, WXK_INSERT );
static EDA_HOTKEY HkDelete( wxT( "Delete Item" ), HK_DELETE, WXK_DELETE );

static EDA_HOTKEY HkFindItem( wxT( "Find Item" ), HK_FIND_ITEM, 'F' + GR_KB_CTRL, ID_FIND_ITEMS );
static EDA_HOTKEY HkFindNextItem( wxT( "Find Next Item" ), HK_FIND_NEXT_ITEM, WXK_F5,
                                  wxEVT_COMMAND_FIND );
static EDA_HOTKEY HkFindNextDrcMarker( wxT( "Find Next DRC Marker" ), HK_FIND_NEXT_DRC_MARKER,
                                       WXK_F5 + GR_KB_SHIFT, EVT_COMMAND_FIND_DRC_MARKER );

// Special keys for library editor:
static EDA_HOTKEY HkCreatePin( wxT( "Create Pin" ), HK_LIBEDIT_CREATE_PIN, 'P' );
static EDA_HOTKEY HkInsertPin( wxT( "Repeat Pin" ), HK_REPEAT_LAST, WXK_INSERT );
static EDA_HOTKEY HkMoveLibItem( wxT( "Move Library Item" ), HK_LIBEDIT_MOVE_GRAPHIC_ITEM, 'M' );


// List of common hotkey descriptors
EDA_HOTKEY* s_Common_Hotkey_List[] =
{
    &HkHelp,
    &HkZoomIn,
    &HkZoomOut,
    &HkZoomRedraw,
    &HkZoomCenter,
    &HkZoomAuto,
    &HkResetLocalCoord,
    &HkUndo,
    &HkRedo,
    NULL
};

// List of hotkey descriptors for schematic
EDA_HOTKEY* s_Schematic_Hotkey_List[] =
{
    &HkFindItem,
    &HkFindNextItem,
    &HkFindNextDrcMarker,
    &HkDelete,
    &HkInsert,
    &HkMove2Drag,
    &HkSaveBlock,
    &HkMove,
    &HkCopyComponentOrText,
    &HkDrag,
    &HkAddComponent,
    &HkAddPower,
    &HkRotate,
    &HkMirrorX,
    &HkMirrorY,
    &HkOrientNormalComponent,
    &HkEdit,
    &HkEditComponentValue,
    &HkEditComponentFootprint,
    &HkBeginWire,
    &HkBeginBus,
    &HkAddLabel,
    &HkAddHierarchicalLabel,
    &HkAddGlobalLabel,
    &HkAddJunction,
    &HkAddNoConn,
    &HkAddHierSheet,
    &HkAddWireEntry,
    &HkAddBusEntry,
    &HkAddGraphicPolyLine,
    &HkAddGraphicText,
    NULL
};

// List of hotkey descriptors for library editor
EDA_HOTKEY* s_LibEdit_Hotkey_List[] =
{
    &HkCreatePin,
    &HkInsertPin,
    &HkEdit,
    &HkMoveLibItem,
    &HkDelete,
    &HkRotate,
    &HkDrag,
    NULL
};

// list of sections and corresponding hotkey list for Eeschema (used to create
// an hotkey config file)
struct EDA_HOTKEY_CONFIG s_Eeschema_Hokeys_Descr[] =
{
    { &g_CommonSectionTag,    s_Common_Hotkey_List,    L"Common keys"           },
    { &g_SchematicSectionTag, s_Schematic_Hotkey_List, L"Schematic editor keys" },
    { &g_LibEditSectionTag,   s_LibEdit_Hotkey_List,   L"library editor keys"   },
    { NULL,                   NULL,                    NULL                     }
};

// list of sections and corresponding hotkey list for the schematic editor
// (used to list current hotkeys)
struct EDA_HOTKEY_CONFIG s_Schematic_Hokeys_Descr[] =
{
    { &g_CommonSectionTag,    s_Common_Hotkey_List,    NULL },
    { &g_SchematicSectionTag, s_Schematic_Hotkey_List, NULL },
    { NULL,                   NULL,                    NULL }
};

// list of sections and corresponding hotkey list for the component editor
// (used to list current hotkeys)
struct EDA_HOTKEY_CONFIG s_Libedit_Hokeys_Descr[] =
{
    { &g_CommonSectionTag,  s_Common_Hotkey_List,  NULL },
    { &g_LibEditSectionTag, s_LibEdit_Hotkey_List, NULL },
    { NULL,                 NULL,                  NULL }
};

// list of sections and corresponding hotkey list for the component browser
// (used to list current hotkeys)
struct EDA_HOTKEY_CONFIG s_Viewlib_Hokeys_Descr[] =
{
    { &g_CommonSectionTag, s_Common_Hotkey_List, NULL },
    { NULL,                NULL,                 NULL }
};

/*
 * Hot keys. Some commands are relative to the item under the mouse cursor
 * Commands are case insensitive
 */
void SCH_EDIT_FRAME::OnHotKey( wxDC* aDC, int aHotKey, const wxPoint& aPosition, EDA_ITEM* aItem )
{
    if( aHotKey == 0 )
        return;

    wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED );

    cmd.SetEventObject( this );

    SCH_SCREEN* screen = GetScreen();

    // itemInEdit == false means no item currently edited. We can ask for editing a new item
    bool itemInEdit = screen->GetCurItem() && screen->GetCurItem()->GetFlags();

    // notBusy == true means no item currently edited and no other command in progress
    // We can change active tool and ask for editing a new item
    bool notBusy = (!itemInEdit) && (screen->m_BlockLocate.m_State == STATE_NO_BLOCK);

    /* Convert lower to upper case (the usual toupper function has problem
     * with non ascii codes like function keys */
    if( (aHotKey >= 'a') && (aHotKey <= 'z') )
        aHotKey += 'A' - 'a';

    // Search command from key :
    EDA_HOTKEY* hotKey = GetDescriptorFromHotkey( aHotKey, s_Common_Hotkey_List );

    if( hotKey == NULL )
        hotKey = GetDescriptorFromHotkey( aHotKey, s_Schematic_Hotkey_List );

    if( hotKey == NULL )
        return;

    switch( hotKey->m_Idcommand )
    {
    default:
    case HK_NOT_FOUND:
        return;

    case HK_HELP:       // Display Current hotkey list
        DisplayHotkeyList( this, s_Schematic_Hokeys_Descr );
        break;

    case HK_RESET_LOCAL_COORD:         /* Reset the relative coord */
        GetScreen()->m_O_Curseur = GetScreen()->GetCrossHairPosition();
        break;

    case HK_ZOOM_IN:
    case HK_ZOOM_OUT:
    case HK_ZOOM_REDRAW:
    case HK_ZOOM_CENTER:
    case HK_ZOOM_AUTO:
        cmd.SetId( hotKey->m_IdMenuEvent );
        GetEventHandler()->ProcessEvent( cmd );
        break;

    case HK_MOVEBLOCK_TO_DRAGBLOCK:   // Switch to drag mode, when block moving
        HandleBlockEndByPopUp( BLOCK_DRAG, aDC );
        break;

    case HK_SAVE_BLOCK:
        HandleBlockEndByPopUp( BLOCK_SAVE, aDC );
        break;

    case HK_DELETE:
        if( notBusy )
            DeleteItemAtCrossHair( aDC );

        break;

    case HK_REPEAT_LAST:
        if( notBusy && m_itemToRepeat && ( m_itemToRepeat->GetFlags() == 0 ) )
            RepeatDrawItem( aDC );

        break;

    case HK_UNDO:
    case HK_REDO:
    case HK_FIND_ITEM:
        if( notBusy )
        {
            cmd.SetId( hotKey->m_IdMenuEvent );
            GetEventHandler()->ProcessEvent( cmd );
        }

        break;

    case HK_FIND_NEXT_ITEM:
    case HK_FIND_NEXT_DRC_MARKER:
        if( notBusy )
        {
            wxFindDialogEvent event( hotKey->m_IdMenuEvent, GetId() );
            event.SetEventObject( this );
            event.SetFlags( m_findReplaceData->GetFlags() );
            event.SetFindString( m_findReplaceData->GetFindString() );
            GetEventHandler()->ProcessEvent( event );
        }

        break;

    case HK_ADD_NEW_COMPONENT:      // Add component
    case HK_ADD_NEW_POWER:          // Add power component
    case HK_ADD_LABEL:
    case HK_ADD_HLABEL:
    case HK_ADD_GLABEL:
    case HK_ADD_JUNCTION:
    case HK_ADD_WIRE_ENTRY:
    case HK_ADD_BUS_ENTRY:
    case HK_ADD_HIER_SHEET:
    case HK_ADD_GRAPHIC_TEXT:
    case HK_ADD_GRAPHIC_POLYLINE:
    case HK_ADD_NOCONN_FLAG:        // Add a no connected flag
    case HK_BEGIN_BUS:
    case HK_BEGIN_WIRE:
        if( notBusy )
        {
            EDA_HOTKEY_CLIENT_DATA data( aPosition );
            cmd.SetInt( aHotKey );
            cmd.SetClientObject( &data );
            cmd.SetId( hotKey->m_IdMenuEvent );
            GetEventHandler()->ProcessEvent( cmd );
        }
        else if( aItem && aItem->IsNew() )
        {
            // If the item is a bus or a wire, a begin command is not possible.
            if( (GetToolId() == ID_BUS_BUTT) && (aItem->Type() == SCH_LINE_T) )
            {
                SCH_LINE* segment = (SCH_LINE*) aItem;

                if( segment->GetLayer() != LAYER_BUS )
                    break;

                // Bus in progress:
                OnLeftClick( aDC, aPosition );
            }
            else if( (GetToolId() == ID_WIRE_BUTT ) && (aItem->Type() == SCH_LINE_T) )
            {
                SCH_LINE* segment = (SCH_LINE*) aItem;

                if( segment->GetLayer() != LAYER_WIRE )
                    break;

                // Wire in progress:
                OnLeftClick( aDC, aPosition );
            }
        }

        break;

    case HK_COPY_COMPONENT_OR_LABEL:        // Duplicate component or text/label
        if( itemInEdit )
            break;

        if( aItem == NULL )
        {
            aItem = LocateAndShowItem( aPosition, SCH_COLLECTOR::MovableItems );

            if( aItem == NULL )
                break;
        }

        cmd.SetId( hotKey->m_IdMenuEvent );
        wxPostEvent( this, cmd );
        break;

    case HK_MIRROR_Y_COMPONENT:             // Mirror Y
    case HK_MIRROR_X_COMPONENT:             // Mirror X
    case HK_ORIENT_NORMAL_COMPONENT:        // Orient 0, no mirror (Component)
    case HK_DRAG:                           // Start drag
    case HK_ROTATE:                         // Rotate schematic item or block.
    case HK_MOVE_COMPONENT_OR_ITEM:         // Start move schematic item.
    case HK_EDIT:                           // Edit schematic item.
    case HK_EDIT_COMPONENT_VALUE:           // Edit component value field.
    case HK_EDIT_COMPONENT_FOOTPRINT:       // Edit component footprint field.
    {
        EDA_HOTKEY_CLIENT_DATA data( aPosition );
        cmd.SetInt( hotKey->m_Idcommand );
        cmd.SetClientObject( &data );
        cmd.SetId( hotKey->m_IdMenuEvent );
        GetEventHandler()->ProcessEvent( cmd );
        break;
    }
    }
}


/*
 * Hot keys for the component editor. Some commands are relatives to the item
 * under the mouse cursor
 * Commands are case insensitive
 */
void LIB_EDIT_FRAME::OnHotKey( wxDC* aDC, int aHotKey, const wxPoint& aPosition, EDA_ITEM* aItem )
{
    if( aHotKey == 0 )
        return;

    wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED );

    cmd.SetEventObject( this );

    bool itemInEdit = m_drawItem && m_drawItem->GetFlags();

    /* Convert lower to upper case (the usual toupper function has problem
     * with non ascii codes like function keys */
    if( (aHotKey >= 'a') && (aHotKey <= 'z') )
        aHotKey += 'A' - 'a';

    EDA_HOTKEY* hotKey = GetDescriptorFromHotkey( aHotKey, s_Common_Hotkey_List );

    if( hotKey == NULL )
        hotKey = GetDescriptorFromHotkey( aHotKey, s_LibEdit_Hotkey_List );

    if( hotKey == NULL )
        return;

    switch( hotKey->m_Idcommand )
    {
    default:
    case HK_NOT_FOUND:
        return;

    case HK_HELP:       // Display Current hotkey list
        DisplayHotkeyList( this, s_Libedit_Hokeys_Descr );
        break;

    case HK_RESET_LOCAL_COORD:         /* Reset the relative coord */
        GetScreen()->m_O_Curseur = GetScreen()->GetCrossHairPosition();
        break;

    case HK_ZOOM_IN:
    case HK_ZOOM_OUT:
    case HK_ZOOM_REDRAW:
    case HK_ZOOM_CENTER:
    case HK_ZOOM_AUTO:
        cmd.SetId( hotKey->m_IdMenuEvent );
        GetEventHandler()->ProcessEvent( cmd );
        break;

    case HK_UNDO:
        if( !itemInEdit )
        {
            cmd.SetId( wxID_UNDO );
            GetEventHandler()->ProcessEvent( cmd );
        }
        break;

    case HK_REDO:
        if( !itemInEdit )
        {
            cmd.SetId( wxID_REDO );
            GetEventHandler()->ProcessEvent( cmd );
        }
        break;

    case HK_REPEAT_LAST:
        if( m_lastDrawItem && (m_lastDrawItem->GetFlags() == 0)
            && ( m_lastDrawItem->Type() == LIB_PIN_T ) )
            RepeatPinItem( aDC, (LIB_PIN*) m_lastDrawItem );
         break;

    case HK_EDIT:
        if( ! itemInEdit )
            m_drawItem = LocateItemUsingCursor( aPosition );

        if( m_drawItem )
        {
            switch( m_drawItem->Type() )
            {
            case LIB_PIN_T:
                cmd.SetId( ID_LIBEDIT_EDIT_PIN );
                GetEventHandler()->ProcessEvent( cmd );
                break;

            case LIB_ARC_T:
            case LIB_CIRCLE_T:
            case LIB_RECTANGLE_T:
            case LIB_POLYLINE_T:
            case LIB_TEXT_T:
                cmd.SetId( ID_POPUP_LIBEDIT_BODY_EDIT_ITEM );
                GetEventHandler()->ProcessEvent( cmd );
                break;

            case LIB_FIELD_T:
                cmd.SetId( ID_POPUP_LIBEDIT_FIELD_EDIT_ITEM );
                GetEventHandler()->ProcessEvent( cmd );
                break;

            default:
                break;
            }
        }
        break;

    case HK_ROTATE:
        if( ! itemInEdit )
            m_drawItem = LocateItemUsingCursor( aPosition );

        if( m_drawItem )
        {
            cmd.SetId( ID_LIBEDIT_ROTATE_ITEM );
            GetEventHandler()->ProcessEvent( cmd );
        }
        break;

    case HK_LIBEDIT_CREATE_PIN:
        SetToolID( ID_LIBEDIT_PIN_BUTT, wxCURSOR_PENCIL, _( "Add Pin" ) );
        OnLeftClick( aDC, aPosition );
        break;

    case HK_DELETE:
        if( !itemInEdit )
            m_drawItem = LocateItemUsingCursor( aPosition );

        if( m_drawItem && !m_drawItem->InEditMode() )
        {
            cmd.SetId( ID_POPUP_LIBEDIT_DELETE_ITEM );
            Process_Special_Functions( cmd );
        }
        break;

    case HK_LIBEDIT_MOVE_GRAPHIC_ITEM:
        if( !itemInEdit )
        {
            m_drawItem = LocateItemUsingCursor( aPosition );

            if( m_drawItem )
            {
                cmd.SetId( ID_POPUP_LIBEDIT_MOVE_ITEM_REQUEST );
                Process_Special_Functions( cmd );
            }
        }
        break;

    case HK_DRAG:
        m_drawItem = LocateItemUsingCursor( aPosition );

        if( m_drawItem && !m_drawItem->InEditMode() )
        {
            cmd.SetId( ID_POPUP_LIBEDIT_MODIFY_ITEM );
            Process_Special_Functions( cmd );
        }
        break;
    }
}
