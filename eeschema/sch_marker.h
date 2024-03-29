/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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

/*
 * @file sch_marker.h
 * @brief SCH_MARKER class definition.
 */

#ifndef TYPE_SCH_MARKER_H_
#define TYPE_SCH_MARKER_H_

#include <sch_item_struct.h>
#include <class_marker_base.h>

/* Marker are mainly used to show an ERC error
 */

enum TypeMarker {
    /* Markers type */
    MARK_UNSPEC,
    MARK_ERC,
    MARK_PCB,
    MARK_SIMUL,
    MARK_NMAX        /* Lats value: end of list */
};


/* Names for corresponding types of markers: */
extern const wxChar* NameMarqueurType[];


class SCH_MARKER : public SCH_ITEM, public MARKER_BASE
{
public:
    SCH_MARKER();

    SCH_MARKER( const wxPoint& aPos, const wxString& aText );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    ~SCH_MARKER();

    virtual wxString GetClass() const
    {
        return wxT( "SCH_MARKER" );
    }

    virtual void Draw( EDA_DRAW_PANEL* aPanel, wxDC* aDC, const wxPoint& aOffset,
                       int aDraw_mode, int aColor = -1 );

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.sch"
     * format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool Save( FILE* aFile ) const;

    /**
     * Function GetBoundingBox
     * returns the orthogonal, bounding box of this object for display purposes.
     * This box should be an enclosing perimeter for visible components of this
     * object, and the units should be in the pcb or schematic coordinate system.
     * It is OK to overestimate the size by a few counts.
     */
    virtual EDA_RECT GetBoundingBox() const;

    // Geometric transforms (used in block operations):

    /** @copydoc SCH_ITEM::Move() */
    virtual void Move( const wxPoint& aMoveVector )
    {
        m_Pos += aMoveVector;
    }


    /** @copydoc SCH_ITEM::MirrorY() */
    virtual void MirrorY( int aYaxis_position );

    /** @copydoc SCH_ITEM::MirrorX() */
    virtual void MirrorX( int aXaxis_position );

    /** @copydoc SCH_ITEM::Rotate() */
    virtual void Rotate( wxPoint aPosition );

    /**
     * Compare DRC marker main and auxiliary text against search string.
     *
     * @param aSearchData - Criteria to search against.
     * @param aFindLocation - a wxPoint where to put the location of matched item. can be NULL.
     * @return True if the DRC main or auxiliary text matches the search criteria.
     */
    virtual bool Matches( wxFindReplaceData& aSearchData, wxPoint * aFindLocation );

    /**
     * Show the marker electronics rule check error on the message panel.
     *
     * @param aFrame - Top window that owns the message panel.
     */
    void DisplayInfo( EDA_DRAW_FRAME* aFrame );

    virtual bool IsSelectStateChanged( const wxRect& aRect );

    /** @copydoc EDA_ITEM::GetSelectMenuText() */
    virtual wxString GetSelectMenuText() const { return wxString( _( "ERC Marker" ) ); }

    /** @copydoc EDA_ITEM::GetMenuImage() */
    virtual BITMAP_DEF GetMenuImage() const { return  erc_xpm; }

    /** @copydoc SCH_ITEM::GetPosition() */
    virtual wxPoint GetPosition() const { return m_Pos; }

    /** @copydoc SCH_ITEM::SetPosition() */
    virtual void SetPosition( const wxPoint& aPosition ) { m_Pos = aPosition; }

    /** @copydoc SCH_ITEM::HitTest(const wxPoint&,int)const */
    virtual bool HitTest( const wxPoint& aPosition, int aAccuracy ) const;

    /** @copydoc EDA_ITEM::Clone() */
    virtual EDA_ITEM* Clone() const;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const;     // override
#endif
};

#endif // TYPE_SCH_MARKER_H_
