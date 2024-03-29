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
 * eeschema/controle.cpp
 */

#include <fctsys.h>
#include <gr_basic.h>
#include <class_drawpanel.h>
#include <eda_dde.h>
#include <wxEeschemaStruct.h>

#include <eeschema_id.h>
#include <general.h>
#include <hotkeys.h>
#include <protos.h>
#include <libeditframe.h>
#include <viewlib_frame.h>
#include <lib_draw_item.h>
#include <lib_pin.h>
#include <sch_sheet.h>
#include <sch_sheet_path.h>
#include <sch_marker.h>
#include <sch_component.h>


SCH_ITEM* SCH_EDIT_FRAME::LocateAndShowItem( const wxPoint& aPosition, const KICAD_T aFilterList[],
                                             int aHotKeyCommandId )
{
    SCH_ITEM*      item;
    wxString       msg;
    LIB_PIN*       Pin     = NULL;
    SCH_COMPONENT* LibItem = NULL;
    wxPoint        gridPosition = GetScreen()->GetNearestGridPosition( aPosition );

    // Check the on grid position first.  There is more likely to be multple items on
    // grid than off grid.
    item = LocateItem( gridPosition, aFilterList, aHotKeyCommandId );

    // If the user aborted the clarification context menu, don't show it again at the
    // off grid position.
    if( !item && m_canvas->GetAbortRequest() )
    {
        m_canvas->SetAbortRequest( false );
        return NULL;
    }

    if( !item && (aPosition != gridPosition) )
        item = LocateItem( aPosition, aFilterList, aHotKeyCommandId );

    if( !item )
    {
        m_canvas->SetAbortRequest( false );  // Just in case the user aborted the context menu.
        return NULL;
    }

    /* Cross probing to Pcbnew if a pin or a component is found */
    switch( item->Type() )
    {
    case SCH_FIELD_T:
    case LIB_FIELD_T:
        LibItem = (SCH_COMPONENT*) item->GetParent();
        SendMessageToPCBNEW( item, LibItem );
        break;

    case SCH_COMPONENT_T:
        LibItem = (SCH_COMPONENT*) item;
        SendMessageToPCBNEW( item, LibItem );
        break;

    case LIB_PIN_T:
        Pin = (LIB_PIN*) item;
        LibItem = (SCH_COMPONENT*) LocateItem( aPosition, SCH_COLLECTOR::ComponentsOnly );
        break;

    default:
        ;
    }

    if( Pin )
    {
        // Force display pin information (the previous display could be a component info)
        Pin->DisplayInfo( this );

        if( LibItem )
            AppendMsgPanel( LibItem->GetRef( m_CurrentSheet ),
                            LibItem->GetField( VALUE )->m_Text, DARKCYAN );

        // Cross probing:2 - pin found, and send a locate pin command to Pcbnew (highlight net)
        SendMessageToPCBNEW( Pin, LibItem );
    }

    return item;
}


SCH_ITEM* SCH_EDIT_FRAME::LocateItem( const wxPoint& aPosition, const KICAD_T aFilterList[],
                                      int aHotKeyCommandId )
{
    SCH_ITEM* item = NULL;

    m_collectedItems.Collect( GetScreen()->GetDrawItems(), aFilterList, aPosition );

    if( m_collectedItems.GetCount() == 0 )
    {
        ClearMsgPanel();
    }
    else if( m_collectedItems.GetCount() == 1 )
    {
        item = m_collectedItems[0];
    }
    else
    {
        // There are certain combinations of items that do not need clarification such as
        // a corner were two lines meet or all the items form a junction.
        if( aHotKeyCommandId )
        {
            switch( aHotKeyCommandId )
            {
            case HK_DRAG:
                if( m_collectedItems.IsCorner() || m_collectedItems.IsNode( false )
                    || m_collectedItems.IsDraggableJunction() )
                {
                    item = m_collectedItems[0];
                }
            default:
                ;
            }
        }

        if( item == NULL )
        {
            wxASSERT_MSG( m_collectedItems.GetCount() <= MAX_SELECT_ITEM_IDS,
                          wxT( "Select item clarification context menu size limit exceeded." ) );

            wxMenu selectMenu;
            wxMenuItem* title = new wxMenuItem( &selectMenu, wxID_NONE, _( "Clarify Selection" ) );

            selectMenu.Append( title );
            selectMenu.AppendSeparator();

            for( int i = 0;  i < m_collectedItems.GetCount() && i < MAX_SELECT_ITEM_IDS;  i++ )
            {
                wxString text = m_collectedItems[i]->GetSelectMenuText();
                BITMAP_DEF xpm = m_collectedItems[i]->GetMenuImage();
                AddMenuItem( &selectMenu, ID_SELECT_ITEM_START + i, text, KiBitmap( xpm ) );
            }

            // Set to NULL in case user aborts the clarification context menu.
            GetScreen()->SetCurItem( NULL );
            m_canvas->SetAbortRequest( true );   // Changed to false if an item is selected
            PopupMenu( &selectMenu );
            m_canvas->MoveCursorToCrossHair();
            item = GetScreen()->GetCurItem();
        }
    }

    GetScreen()->SetCurItem( item );

    if( item )
        item->DisplayInfo( this );
    else
        ClearMsgPanel();

    return item;
}


void SCH_EDIT_FRAME::GeneralControl( wxDC* aDC, const wxPoint& aPosition, int aHotKey )
{
    wxRealPoint gridSize;
    SCH_SCREEN* screen = GetScreen();
    wxPoint     oldpos;
    wxPoint     pos = aPosition;

    pos = screen->GetNearestGridPosition( pos );
    oldpos = screen->GetCrossHairPosition();
    gridSize = screen->GetGridSize();

    switch( aHotKey )
    {
    case 0:
        break;

    case WXK_NUMPAD8:
    case WXK_UP:
        pos.y -= wxRound( gridSize.y );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD2:
    case WXK_DOWN:
        pos.y += wxRound( gridSize.y );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD4:
    case WXK_LEFT:
        pos.x -= wxRound( gridSize.x );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD6:
    case WXK_RIGHT:
        pos.x += wxRound( gridSize.x );
        m_canvas->MoveCursor( pos );
        break;

    default:
        break;
    }

    // Update cursor position.
    screen->SetCrossHairPosition( pos );

    if( oldpos != screen->GetCrossHairPosition() )
    {
        pos = screen->GetCrossHairPosition();
        screen->SetCrossHairPosition( oldpos );
        m_canvas->CrossHairOff( aDC );
        screen->SetCrossHairPosition( pos );
        m_canvas->CrossHairOn( aDC );

        if( m_canvas->IsMouseCaptured() )
        {
#ifdef USE_WX_OVERLAY
            wxDCOverlay oDC( m_overlay, (wxWindowDC*)aDC );
            oDC.Clear();
            m_canvas->CallMouseCapture( aDC, aPosition, false );
#else
            m_canvas->CallMouseCapture( aDC, aPosition, true );
#endif
        }
#ifdef USE_WX_OVERLAY
        else
        {
            m_overlay.Reset();
        }
#endif
    }

    if( aHotKey )
    {
        if( screen->GetCurItem() && screen->GetCurItem()->GetFlags() )
            OnHotKey( aDC, aHotKey, aPosition, screen->GetCurItem() );
        else
            OnHotKey( aDC, aHotKey, aPosition, NULL );
    }

    UpdateStatusBar();    /* Display cursor coordinates info */
}


void LIB_EDIT_FRAME::GeneralControl( wxDC* aDC, const wxPoint& aPosition, int aHotKey )
{
    wxRealPoint gridSize;
    SCH_SCREEN* screen = GetScreen();
    wxPoint     oldpos;
    wxPoint     pos = aPosition;

    pos = screen->GetNearestGridPosition( pos );
    oldpos = screen->GetCrossHairPosition();
    gridSize = screen->GetGridSize();

    switch( aHotKey )
    {
    case 0:
        break;

    case WXK_NUMPAD8:
    case WXK_UP:
        pos.y -= wxRound( gridSize.y );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD2:
    case WXK_DOWN:
        pos.y += wxRound( gridSize.y );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD4:
    case WXK_LEFT:
        pos.x -= wxRound( gridSize.x );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD6:
    case WXK_RIGHT:
        pos.x += wxRound( gridSize.x );
        m_canvas->MoveCursor( pos );
        break;

    default:
        break;
    }

    // Update the cursor position.
    screen->SetCrossHairPosition( pos );

    if( oldpos != screen->GetCrossHairPosition() )
    {
        pos = screen->GetCrossHairPosition();
        screen->SetCrossHairPosition( oldpos );
        m_canvas->CrossHairOff( aDC );
        screen->SetCrossHairPosition( pos );
        m_canvas->CrossHairOn( aDC );

        if( m_canvas->IsMouseCaptured() )
        {
#ifdef USE_WX_OVERLAY
            wxDCOverlay oDC( m_overlay, (wxWindowDC*)aDC );
            oDC.Clear();
            m_canvas->CallMouseCapture( aDC, aPosition, false );
#else
            m_canvas->CallMouseCapture( aDC, aPosition, true );
#endif
        }
#ifdef USE_WX_OVERLAY
        else
        {
            m_overlay.Reset();
        }
#endif
    }

    if( aHotKey )
    {
        OnHotKey( aDC, aHotKey, aPosition, NULL );
    }

    UpdateStatusBar();
}


void LIB_VIEW_FRAME::GeneralControl( wxDC* aDC, const wxPoint& aPosition, int aHotKey )
{
    wxRealPoint gridSize;
    SCH_SCREEN* screen = GetScreen();
    wxPoint     oldpos;
    wxPoint     pos = aPosition;

    pos = screen->GetNearestGridPosition( pos );
    oldpos = screen->GetCrossHairPosition();
    gridSize = screen->GetGridSize();

    switch( aHotKey )
    {
    case 0:
        break;

    case WXK_NUMPAD8:
    case WXK_UP:
        pos.y -= wxRound( gridSize.y );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD2:
    case WXK_DOWN:
        pos.y += wxRound( gridSize.y );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD4:
    case WXK_LEFT:
        pos.x -= wxRound( gridSize.x );
        m_canvas->MoveCursor( pos );
        break;

    case WXK_NUMPAD6:
    case WXK_RIGHT:
        pos.x += wxRound( gridSize.x );
        m_canvas->MoveCursor( pos );
        break;

    default:
        break;
    }

    // Update cursor position.
    screen->SetCrossHairPosition( pos );

    if( oldpos != screen->GetCrossHairPosition() )
    {
        pos = screen->GetCrossHairPosition();
        screen->SetCrossHairPosition( oldpos );
        m_canvas->CrossHairOff( aDC );
        screen->SetCrossHairPosition( pos );
        m_canvas->CrossHairOn( aDC );

        if( m_canvas->IsMouseCaptured() )
        {
            m_canvas->CallMouseCapture( aDC, aPosition, true );
        }
    }

    if( aHotKey )
    {
        if( screen->GetCurItem() && screen->GetCurItem()->GetFlags() )
            OnHotKey( aDC, aHotKey, aPosition, screen->GetCurItem() );
        else
            OnHotKey( aDC, aHotKey, aPosition, NULL );
    }

    UpdateStatusBar();
}
