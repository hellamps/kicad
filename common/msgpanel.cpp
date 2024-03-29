/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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

#ifdef __GNUG__
#pragma implementation
#endif

#include <fctsys.h>
#include <wxstruct.h>
#include <common.h>
#include <colors.h>


BEGIN_EVENT_TABLE( EDA_MSG_PANEL, wxPanel )
    EVT_PAINT( EDA_MSG_PANEL::OnPaint )
END_EVENT_TABLE()


EDA_MSG_PANEL::EDA_MSG_PANEL( EDA_DRAW_FRAME* parent, int id,
                              const wxPoint& pos, const wxSize& size ) :
    wxPanel( parent, id, pos, size )
{
    SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ) );
    SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
    m_last_x = 0;

    m_fontSize = computeFontSize();
}


EDA_MSG_PANEL::~EDA_MSG_PANEL()
{
}


wxSize EDA_MSG_PANEL::computeFontSize()
{
    // Get size of the wxSYS_DEFAULT_GUI_FONT
    wxSize      fontSizeInPixels;

    wxScreenDC dc;

    dc.SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ) );
    dc.GetTextExtent( wxT( "W" ), &fontSizeInPixels.x, &fontSizeInPixels.y );

    return fontSizeInPixels;
}


int EDA_MSG_PANEL::GetRequiredHeight()
{
    // make space for two rows of text plus a number of pixels between them.
    return 2 * computeFontSize().y + 0;
}


wxSize EDA_MSG_PANEL::computeTextSize( const wxString& text )
{
    // Get size of the wxSYS_DEFAULT_GUI_FONT
    wxSize      textSizeInPixels;

    wxScreenDC dc;

    dc.SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ) );
    dc.GetTextExtent( text, &textSizeInPixels.x, &textSizeInPixels.y );

    return textSizeInPixels;
}


void EDA_MSG_PANEL::OnPaint( wxPaintEvent& event )
{
    wxPaintDC dc( this );

    erase( &dc );

    dc.SetBackground( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
    dc.SetBackgroundMode( wxSOLID );
    dc.SetTextBackground( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
    dc.SetFont( wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT ) );

    for( unsigned i=0;  i<m_Items.size();  ++i )
        showItem( dc, m_Items[i] );

    event.Skip();
}

void EDA_MSG_PANEL::AppendMessage( const wxString& textUpper,
                                   const wxString& textLower,
                                   int color, int pad )
{
    wxString    text;
    wxSize      drawSize = GetClientSize();

    text = ( textUpper.Len() > textLower.Len() ) ? textUpper : textLower;
    text.Append( ' ', pad );

    EDA_MSG_ITEM item;

    /* Don't put the first message a window client position 0.  Offset by
     * one 'W' character width. */
    if( m_last_x == 0 )
        m_last_x = m_fontSize.x;

    item.m_X = m_last_x;

    item.m_UpperY = ( drawSize.y / 2 ) - m_fontSize.y;
    item.m_LowerY = drawSize.y - m_fontSize.y;

    item.m_UpperText = textUpper;
    item.m_LowerText = textLower;
    item.m_Color = color;
    m_Items.push_back( item );
    m_last_x += computeTextSize( text ).x;

    // Add an extra space between texts for a better look:
    m_last_x += m_fontSize.x;

    Refresh();
}


void EDA_MSG_PANEL::SetMessage( int aXPosition, const wxString& aUpperText,
                                const wxString& aLowerText, int aColor )
{
    wxPoint pos;
    wxSize drawSize = GetClientSize();

    if( aXPosition >= 0 )
        m_last_x = pos.x = aXPosition * (m_fontSize.x + 2);
    else
        pos.x = m_last_x;

    EDA_MSG_ITEM item;

    item.m_X = pos.x;

    item.m_UpperY = (drawSize.y / 2) - m_fontSize.y;
    item.m_LowerY = drawSize.y - m_fontSize.y;

    item.m_UpperText = aUpperText;
    item.m_LowerText = aLowerText;
    item.m_Color = aColor;

    int ndx;

    // update the vector, which is sorted by m_X
    int limit = m_Items.size();

    for( ndx=0;  ndx<limit;  ++ndx )
    {
        // replace any item with same X
        if( m_Items[ndx].m_X == item.m_X )
        {
            m_Items[ndx] = item;
            break;
        }

        if( m_Items[ndx].m_X > item.m_X )
        {
            m_Items.insert( m_Items.begin() + ndx, item );
            break;
        }
    }

    if( ndx == limit )        // mutually exclusive with two above if tests
    {
        m_Items.push_back( item );
    }

    Refresh();
}


void EDA_MSG_PANEL::showItem( wxDC& dc, const EDA_MSG_ITEM& aItem )
{
    int color = aItem.m_Color;

    if( color >= 0 )
    {
        color &= MASKCOLOR;
        dc.SetTextForeground( wxColour( ColorRefs[color].m_Red,
                                        ColorRefs[color].m_Green,
                                        ColorRefs[color].m_Blue ) );
    }

    if( !aItem.m_UpperText.IsEmpty() )
    {
        dc.DrawText( aItem.m_UpperText, aItem.m_X, aItem.m_UpperY );
    }

    if( !aItem.m_LowerText.IsEmpty() )
    {
        dc.DrawText( aItem.m_LowerText, aItem.m_X, aItem.m_LowerY );
    }
}


void EDA_MSG_PANEL::EraseMsgBox()
{
   m_Items.clear();
   m_last_x = 0;
   Refresh();
}


void EDA_MSG_PANEL::erase( wxDC* DC )
{
    wxPen   pen;
    wxBrush brush;

    wxSize  size  = GetClientSize();
    wxColor color = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );

    pen.SetColour( color );

    brush.SetColour( color );
    brush.SetStyle( wxSOLID );

    DC->SetPen( pen );
    DC->SetBrush( brush );

    DC->DrawRectangle( 0, 0, size.x, size.y );
}
