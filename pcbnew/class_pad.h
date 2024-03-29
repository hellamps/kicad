/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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
 * @file class_pad.h
 * @brief Pad object description
 */

#ifndef PAD_H_
#define PAD_H_


#include <class_board_item.h>
#include <class_board_connected_item.h>
#include <pad_shapes.h>
#include <PolyLine.h>
#include <param_config.h>       // PARAM_CFG_ARRAY
#include "zones.h"

class LINE_READER;
class EDA_3D_CANVAS;
class EDA_DRAW_PANEL;
class MODULE;
class TRACK;


/* Default layers used for pads, according to the pad type.
 * this is default values only, they can be changed for a given pad
 */

// PAD_STANDARD:
#define PAD_STANDARD_DEFAULT_LAYERS ALL_CU_LAYERS | SILKSCREEN_LAYER_FRONT | \
    SOLDERMASK_LAYER_BACK | SOLDERMASK_LAYER_FRONT

// PAD_CONN:
#define PAD_CONN_DEFAULT_LAYERS LAYER_FRONT | SOLDERPASTE_LAYER_FRONT | SOLDERMASK_LAYER_FRONT

// PAD_SMD:
#define PAD_SMD_DEFAULT_LAYERS LAYER_FRONT | SOLDERMASK_LAYER_FRONT

//PAD_HOLE_NOT_PLATED:
#define PAD_HOLE_NOT_PLATED_DEFAULT_LAYERS ALL_CU_LAYERS | SILKSCREEN_LAYER_FRONT | \
    SOLDERMASK_LAYER_BACK | SOLDERMASK_LAYER_FRONT


// Helper class to store parameters used to draw a pad
class PAD_DRAWINFO
{
public:
    EDA_DRAW_PANEL* m_DrawPanel;  // the EDA_DRAW_PANEL used to draw a PAD ; can be null
    int m_DrawMode;               // the draw mode
    int m_Color;                  // color used to draw the pad shape , from pad layers and
                                  // visible layers
    int m_HoleColor;              // color used to draw the pad hole
    int m_NPHoleColor;            // color used to draw a pad Not Plated hole
    int m_PadClearance;           // clearance value, used to draw the pad area outlines
    wxSize m_Mask_margin;         // margin, used to draw solder paste when only one layer is shown
    bool m_Display_padnum;        // true to show pad number
    bool m_Display_netname;       // true to show net name
    bool m_ShowPadFilled;         // true to show pad as solid area, false to show pas in
                                  // sketch mode
    bool m_ShowNCMark;            // true to show pad not connected mark
    bool m_ShowNotPlatedHole;     // true when the pad hole in not plated, to draw a specific
                                  // pad shape
    bool m_IsPrinting;            // true to print, false to display on screen.
    wxPoint m_Offset;             // general draw offset

    PAD_DRAWINFO();
};


class D_PAD : public BOARD_CONNECTED_ITEM
{
public:
    static int  m_PadSketchModePenSize; ///< Pen size used to draw pads in sketch mode
                                        ///< (mode used to print pads on silkscreen layer)

public:
    D_PAD( MODULE* parent );

    // Do not create a copy constructor.  The one generated by the compiler is adequate.

    void Copy( D_PAD* source );

    D_PAD* Next() const       { return (D_PAD*) Pnext; }

    MODULE* GetParent() const { return (MODULE*) m_Parent; }

    void SetPadName( const wxString& name );    // Change pad name
    const wxString GetPadName() const;

    bool PadNameEqual( const D_PAD* other ) const
    {
        return m_NumPadName == other->m_NumPadName; // hide tricks behind sensible API
    }

    /**
     * Function SetNetname
     * @param aNetname: the new netname
     */
    void SetNetname( const wxString& aNetname );

    /**
     * Function GetNetname
     * @return const wxString& - the full netname
     */
    const wxString& GetNetname() const { return m_Netname; }

    /**
     * Function GetShortNetname
     * @return const wxString& - the short netname
     */
    const wxString& GetShortNetname() const { return m_ShortNetname; }

    /**
     * Function GetShape
     * @return the shape of this pad.
     */
    PAD_SHAPE_T GetShape() const                { return m_PadShape; }
    void SetShape( PAD_SHAPE_T aShape )         { m_PadShape = aShape; m_boundingRadius = -1; }

    void SetPosition( const wxPoint& aPos )     { m_Pos = aPos; }   // was overload
    const wxPoint& GetPosition() const          { return m_Pos; }   // was overload

    void SetY( int y )                          { m_Pos.y = y; }
    void SetX( int x )                          { m_Pos.x = x; }

    void SetPos0( const wxPoint& aPos )         { m_Pos0 = aPos; }
    const wxPoint& GetPos0() const              { return m_Pos0; }

    void SetY0( int y )                         { m_Pos0.y = y; }
    void SetX0( int x )                         { m_Pos0.x = x; }

    void SetSize( const wxSize& aSize )         { m_Size = aSize;  m_boundingRadius = -1; }
    const wxSize& GetSize() const               { return m_Size; }

    void SetDelta( const wxSize& aSize )        { m_DeltaSize = aSize;  m_boundingRadius = -1; }
    const wxSize& GetDelta() const              { return m_DeltaSize; }

    void SetDrillSize( const wxSize& aSize )    { m_Drill = aSize; }
    const wxSize& GetDrillSize() const          { return m_Drill; }

    void SetOffset( const wxPoint& aOffset )    { m_Offset = aOffset; }
    const wxPoint& GetOffset() const            { return m_Offset; }

    /**
     * Function Flip
     * flips this pad to the other outter most copper layer, back to front or
     * vice versa, and does this vertically, so the x coordinate is not affected.
     *
     * @param aTranslationY is the contribution of my 'y' position provided by
     *  my parent module.
     */
    void Flip( int aTranslationY );

    /**
     * Function SetOrientation
     * sets the rotation angle of the pad.
     * @param aAngle is tenths of degrees, but will soon be degrees.  If it is
     *  outside of 0 - 3600, then it will be normalized before being saved.
     */
    void SetOrientation( double aAngle );

    /**
     * Function GetOrientation
     * returns the rotation angle of the pad in tenths of degress, but soon degrees.
     */
    double  GetOrientation() const { return m_Orient; }

    void SetDrillShape( PAD_SHAPE_T aDrillShape ) { m_DrillShape = aDrillShape; }
    PAD_SHAPE_T GetDrillShape() const           { return m_DrillShape; }

    void SetLayerMask( int aLayerMask )         { m_layerMask = aLayerMask; }
    int GetLayerMask() const                    { return m_layerMask; }

    void SetAttribute( PAD_ATTR_T aAttribute )  { m_Attribute = aAttribute; }
    PAD_ATTR_T GetAttribute() const             { return m_Attribute; }

    void SetDieLength( int aLength )            { m_LengthDie = aLength; }
    int GetDieLength() const                    { return m_LengthDie; }

    int GetLocalSolderMaskMargin() const        { return m_LocalSolderMaskMargin; }
    void SetLocalSolderMaskMargin( int aMargin ) { m_LocalSolderMaskMargin = aMargin; }

    int GetLocalClearance() const               { return m_LocalClearance; }
    void SetLocalClearance( int aClearance )    { m_LocalClearance = aClearance; }

    int GetLocalSolderPasteMargin() const       { return m_LocalSolderPasteMargin; }
    void SetLocalSolderPasteMargin( int aMargin ) { m_LocalSolderPasteMargin = aMargin; }

    double GetLocalSolderPasteMarginRatio() const { return m_LocalSolderPasteMarginRatio; }
    void SetLocalSolderPasteMarginRatio( double aRatio ) { m_LocalSolderPasteMarginRatio = aRatio; }


    /**
     * Function TransformShapeWithClearanceToPolygon
     * Convert the pad shape to a closed polygon
     * Used in filling zones calculations
     * Circles and arcs are approximated by segments
     * @param aCornerBuffer = a buffer to store the polygon
     * @param aClearanceValue = the clearance around the pad
     * @param aCircleToSegmentsCount = the number of segments to approximate a circle
     * @param aCorrectionFactor = the correction to apply to circles radius to keep
     * clearance when the circle is approximated by segment bigger or equal
     * to the real clearance value (usually near from 1.0)
    */
    void TransformShapeWithClearanceToPolygon( std::vector <CPolyPt>& aCornerBuffer,
                                               int aClearanceValue,
                                               int aCircleToSegmentsCount,
                                               double aCorrectionFactor );

     /**
     * Function GetClearance
     * returns the clearance in internal units.  If \a aItem is not NULL then the
     * returned clearance is the greater of this object's clearance and
     * aItem's clearance.  If \a aItem is NULL, then this objects clearance
     * is returned.
     * @param aItem is another BOARD_CONNECTED_ITEM or NULL
     * @return int - the clearance in internal units.
     */
    virtual int GetClearance( BOARD_CONNECTED_ITEM* aItem = NULL ) const;

   // Mask margins handling:

    /**
     * Function GetSolderMaskMargin
     * @return the margin for the solder mask layer
     * usually > 0 (mask shape bigger than pad
     * value is
     * 1 - the local value
     * 2 - if null, the parent footprint value
     * 1 - if null, the global value
     */
    int GetSolderMaskMargin();

    /**
     * Function GetSolderPasteMargin
     * @return the margin for the solder mask layer
     * usually < 0 (mask shape smaller than pad
     * because the margin can be dependent on the pad size, the margin has a x and a y value
     * value is
     * 1 - the local value
     * 2 - if null, the parent footprint value
     * 1 - if null, the global value
     */
    wxSize GetSolderPasteMargin();

    void SetZoneConnection( ZoneConnection aType ) { m_ZoneConnection = aType; }
    ZoneConnection GetZoneConnection() const;

    void SetThermalWidth( int aWidth ) { m_ThermalWidth = aWidth; }
    int GetThermalWidth() const;

    void SetThermalGap( int aGap ) { m_ThermalGap = aGap; }
    int GetThermalGap() const;

    /* Reading and writing data on files */
    int ReadDescr( LINE_READER* aReader );

    /**
     * Function Save
     * writes the data structures for this object out to a FILE in "*.brd" format.
     * @param aFile The FILE to write to.
     * @return bool - true if success writing else false.
     */
    bool Save( FILE* aFile ) const;


    /* drawing functions */
    /**
     * Draw a pad:
     * @param aPanel = the EDA_DRAW_PANEL panel
     * @param aDC = the current device context
     * @param aDrawMode = mode: GR_OR, GR_XOR, GR_AND...
     * @param aOffset = draw offset
     */
    void Draw( EDA_DRAW_PANEL* aPanel, wxDC* aDC,
               int aDrawMode, const wxPoint& aOffset = ZeroOffset );

    void Draw3D( EDA_3D_CANVAS* glcanvas );

    /**
     * Function DrawShape
     * basic function to draw a pad.
     * <p>
     * This function is used by Draw after calculation of parameters (color, ) final
     * orientation transforms are set. It can also be called to draw a pad on any panel
     * even if this panel is not a EDA_DRAW_PANEL for instance on a wxPanel inside the
     * pad editor.
     * </p>
     */
    void DrawShape( EDA_RECT* aClipBox, wxDC* aDC, PAD_DRAWINFO& aDrawInfo );

    /**
     * Function BuildPadPolygon
     * Has meaning only for polygonal pads (trapezoid and rectangular)
     * Build the Corner list of the polygonal shape,
     * depending on shape, extra size (clearance ...) and orientation
     * @param aCoord = a buffer to fill (4 corners).
     * @param aInflateValue = wxSize: the clearance or margin value. value > 0:
     *                        inflate, < 0 deflate
     * @param aRotation = full rotation of the polygon
     */
    void BuildPadPolygon( wxPoint aCoord[4], wxSize aInflateValue, int aRotation ) const;

    /**
     * Function BuildSegmentFromOvalShape
     * Has meaning only for OVAL (and ROUND) pads
     * Build an equivalent segment having the same shape as the OVAL shape,
     * Useful in draw function and in DRC and HitTest functions,
     *  because segments are already well handled by track tests
     * @param aSegStart = the starting point of the equivalent segment relative to the shape
     *                    position.
     * @param aSegEnd = the ending point of the equivalent segment, relative to the shape position
     * @param aRotation = full rotation of the segment
     * @return the width of the segment
     */
    int BuildSegmentFromOvalShape( wxPoint& aSegStart, wxPoint& aSegEnd, int aRotation ) const;

    void ReturnStringPadName( wxString& text ) const; // Return pad name as string in a buffer

    /**
     * Function GetBoundingRadius
     * returns the radius of a minimum sized circle which fully encloses this pad.
     */
    int GetBoundingRadius()
    {
        // Any member function which would affect this calculation should set
        // m_boundingRadius to -1 to re-trigger the calculation from here.
        // Currently that is only m_Size, m_DeltaSize, and m_PadShape accessors.
        if( m_boundingRadius == -1 )
        {
            m_boundingRadius = boundingRadius();
        }

        return m_boundingRadius;
    }

    const wxPoint ReturnShapePos() const;

    /**
     * Function GetSubRatsnest
     * @return int - the netcode
     */
    int GetSubRatsnest() const { return m_SubRatsnest; }
    void SetSubRatsnest( int aSubRatsnest ) { m_SubRatsnest = aSubRatsnest; }


    /**
     * Function DisplayInfo
     * has knowledge about the frame and how and where to put status information
     * about this object into the frame's message panel.
     * Is virtual from EDA_ITEM.
     * @param frame A EDA_DRAW_FRAME in which to print status information.
     */
    void DisplayInfo( EDA_DRAW_FRAME* frame );

    /**
     * Function IsOnLayer
     * tests to see if this object is on the given layer.  Is virtual so
     * objects like D_PAD, which reside on multiple layers can do their own
     * form of testing.
     * @param aLayer The layer to test for.
     * @return bool - true if on given layer, else false.
     */
    bool IsOnLayer( int aLayer ) const;

    /** @copydoc EDA_ITEM::HitTest(const wxPoint&) */
    virtual bool HitTest( const wxPoint& aPosition );

    /**
     * Function GetClass
     * returns the class name.
     * @return wxString
     */
    virtual wxString GetClass() const
    {
        return wxT( "PAD" );
    }

    /**
     * Function GetBoundingBox
     * returns the bounding box of this pad
     * Mainly used to redraw the screen area occupied by the pad
     */
    EDA_RECT GetBoundingBox() const;

    /**
     * Function Compare
     * compares two pads and return 0 if they are equal.
     * @return int - <0 if left less than right, 0 if equal, >0 if left greater than right.
     */
    static int Compare( const D_PAD* padref, const D_PAD* padcmp );

    /**
     * Function Move
     * move this object.
     * @param aMoveVector - the move vector for this object.
     */
    virtual void Move( const wxPoint& aMoveVector )
    {
        m_Pos += aMoveVector;
    }


    virtual wxString GetSelectMenuText() const;

    virtual BITMAP_DEF GetMenuImage() const { return pad_xpm; }

    /**
     * Function ShowPadShape
     * @return the name of the shape
     */
    wxString ShowPadShape() const;

    /**
     * Function ShowPadAttr
     * @return the name of the pad type (attribute) : STD, SMD ...
     */
    wxString ShowPadAttr() const;

    /**
     * Function AppendConfigs
     * appends to @a aResult the configuration setting accessors which will later
     * allow reading or writing of configuration file information directly into
     * this object.
     */
    void AppendConfigs( PARAM_CFG_ARRAY* aResult );

    /** @copydoc EDA_ITEM::Clone() */
    virtual EDA_ITEM* Clone() const;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const;     // overload
#endif


private:
    /**
     * Function boundingRadius
     * returns a calculated radius of a bounding circle for this pad.
     */
    int boundingRadius() const;

    int         m_boundingRadius;  ///< radius of the circle containing the pad shape


    wxString    m_Netname;         ///< Full net name like /mysheet/mysubsheet/vout used by Eeschema
    wxString    m_ShortNetname;    ///< short net name, like vout from /mysheet/mysubsheet/vout

    /// Pad name (4 char) or a long identifier (used in pad name
    /// comparisons because this is faster than string comparison)
    union
    {
#define PADNAMEZ    4
        char        m_Padname[PADNAMEZ];    // zero padded at end to full size
        wxUint32    m_NumPadName;           // same number of bytes as m_Padname[]
    };

    wxPoint     m_Pos;              ///< pad Position on board

    PAD_SHAPE_T m_PadShape;         ///< Shape: PAD_CIRCLE, PAD_RECT, PAD_OVAL, PAD_TRAPEZOID


    int         m_SubRatsnest;      ///< variable used in rats nest computations
                                    ///< handle subnet (block) number in ratsnest connection

    wxSize      m_Drill;            ///< Drill diam (drill shape = PAD_CIRCLE) or drill size
                                    ///< (shape = OVAL) for drill shape = PAD_CIRCLE, drill
                                    ///< diam = m_Drill.x

    wxSize      m_Size;             ///< X and Y size ( relative to orient 0)

    PAD_SHAPE_T m_DrillShape;       ///< Shape PAD_CIRCLE, PAD_OVAL

    /**
     * m_Offset is useful only for oblong pads (it can be used for other
     * shapes, but without any interest).
     * This is the offset between the pad hole and the pad shape (you must
     * understand here pad shape = copper area around the hole)
     * Most of cases, the hole is the center of the shape (m_Offset = 0).
     * But some board designers use oblong pads with a hole moved to one of the
     * oblong pad shape ends.
     * In all cases the pad position is the pad hole.
     * The physical shape position (used to draw it for instance) is pad
     * position (m_Pos) + m_Offset.
     * D_PAD::ReturnShapePos() returns the physical shape position according to
     * the offset and the pad rotation.
     */
    wxPoint     m_Offset;

    int         m_layerMask;        ///< Bitwise layer :1= copper layer, 15= cmp,
                                    ///< 2..14 = internal layers
                                    ///< 16 .. 31 = technical layers

    wxSize      m_DeltaSize;        ///< delta on rectangular shapes

    wxPoint     m_Pos0;             ///< Initial Pad position (i.e. pad position relative to the
                                    ///< module anchor, orientation 0)

    PAD_ATTR_T  m_Attribute;        ///< NORMAL, PAD_SMD, PAD_CONN, PAD_HOLE_NOT_PLATED
    double      m_Orient;           ///< in 1/10 degrees

    int         m_LengthDie;            ///< Length net from pad to die on chip

    /// Local clearance. When null, the module default value is used.
    /// when the module default value is null, the netclass value is used
    /// Usually the local clearance is null
    int         m_LocalClearance;

    // Local mask margins: when 0, the parent footprint design values are used

    int         m_LocalSolderMaskMargin;        ///< Local solder mask margin
    int         m_LocalSolderPasteMargin;       ///< Local solder paste margin absolute value

    double      m_LocalSolderPasteMarginRatio;  ///< Local solder mask margin ratio value of pad size
                                                ///< The final margin is the sum of these 2 values
    ZoneConnection m_ZoneConnection;
    int m_ThermalWidth, m_ThermalGap;
};

#endif  // PAD_H_
