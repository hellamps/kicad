/**
 * @file gpcb_exchange.cpp
 * @brief Import functions to import footprints from a gpcb (Newlib) library.
 */

#include <fctsys.h>
#include <wxstruct.h>
#include <kicad_string.h>
#include <trigo.h>
#include <richio.h>
#include <filter_reader.h>
#include <macros.h>

#include <class_pad.h>
#include <class_module.h>
#include <class_edge_mod.h>

#include <pcbnew.h>


/* read parameters from a line, and return all params in a wxArrayString
 * each param is in one wxString, and double quotes removed if exists
 */
static void Extract_Parameters( wxArrayString& param_list, char* text );
static bool TestFlags( const wxString& flg_string, long flg_mask, const wxChar* flg_name );


/**
 * Function Read_GPCB_Descr
 * Read a footprint description in GPCB (Newlib) format
 * @param CmpFullFileName = Full file name (there is one footprint per file.
 * this is also the footprint name
 * @return bool - true if success reading else false.
 */

/* a sample is
 *
 * Element["" "" "" "" 29000 44000 0 0 0 100 ""]
 * (
 * Pad[-5000 0 -4000 0 4999 0 4999 "" "1" "square"]
 * Pad[4000 0 5000 0 4999 0 4999 "" "2" "square,edge2"]
 * ElementLine [8000 3000 1000 3000 199]
 * ElementLine [8000 -3000 8000 3000 199]
 * ElementLine [-8000 3000 -1000 3000 199]
 * ElementLine [-8000 -3000 -1000 -3000 199]
 * ElementLine [8000 -3000 1000 -3000 199]
 * ElementLine [-8000 -3000 -8000 3000 199]
 * )
 *
 * Format
 * Element [SFlags "Desc" "Name" "Value" MX MY TX TY TDir TScale TSFlags]
 * Element (NFlags "Desc" "Name" "Value" MX MY TX TY TDir TScale TNFlags)
 * Element (NFlags "Desc" "Name" "Value" TX TY TDir TScale TNFlags)
 * Element (NFlags "Desc" "Name" TX TY TDir TScale TNFlags)
 * Element ("Desc" "Name" TX TY TDir TScale TNFlags)
 * (
 * . . . contents . . . *
 * )
 * With:
 * SFlags Symbolic or numeric flags, for the element as a whole.
 * NFlags Numeric flags, for the element as a whole.
 * Desc The description of the element. This is one of the three strings which can be
 * displayed on the screen.
 * Name The name of the element, usually the reference designator.
 * Value The value of the element.
 * MX MY The location of the element�s mark. This is the reference point for placing the element and its pins and pads.
 * TX TY The upper left corner of the text (one of the three strings).
 * TDir The relative direction of the text. 0 means left to right for an unrotated element, 1 means up, 2 left, 3 down.
 * TScale Size of the text, as a percentage of the �default� size of of the font (the default font is about 40 mils high). Default is 100 (40 mils).
 * TSFlags Symbolic or numeric flags, for the text.
 * TNFlags Numeric flags, for the text.
 *
 * Elements may contain pins, pads, element
 *
 * ElementLine [X1 Y1 X2 Y2 Thickness]
 * ElementLine (X1 Y1 X2 Y2 Thickness)
 *
 * ElementArc [X Y Width Height StartAngle DeltaAngle Thickness]
 * ElementArc (X Y Width Height StartAngle DeltaAngle Thickness)
 *   (rotation in clockwise)
 *   ( Note: Pad is a SMD Pad in Pcbnew, and Pin is a through hole Pad in Pcbnew )
 * Pad [rX1 rY1 rX2 rY2 Thickness Clearance Mask "Name" "Number" SFlags]
 * Pad (rX1 rY1 rX2 rY2 Thickness Clearance Mask "Name" "Number" NFlags)
 * Pad (aX1 aY1 aX2 aY2 Thickness "Name" "Number" NFlags)
 * Pad (aX1 aY1 aX2 aY2 Thickness "Name" NFlags)
 *
 * Pin [rX rY Thickness Clearance Mask Drill "Name" "Number" SFlags]
 * Pin (rX rY Thickness Clearance Mask Drill "Name" "Number" NFlags)
 * Pin (aX aY Thickness Drill "Name" "Number" NFlags)
 * Pin (aX aY Thickness Drill "Name" NFlags)
 * Pin (aX aY Thickness "Name" NFlags)
 *
 * Object Flags :
 *
 * Note that object flags can be given numerically (like 0x0147) or symbolically (like
 * "found,showname,square". Some numeric values are reused for different object types.
 * The table below lists the numeric value followed by the symbolic name.
 * 0x0001 pin
 * If set, this object is a pin. This flag is for internal use only.
 * 0x0002 via
 * Likewise, for vias.
 * 0x0004 found
 * If set, this object has been found by FindConnection().
 * 0x0008 hole
 * For pins and vias, this flag means that the pin or via is a hole without a copper
 * annulus.
 * 0x0010 rat
 * If set for a line, indicates that this line is a rat line instead of a copper trace.
 * 0x0010 pininpoly
 * For pins and pads, this flag is used internally to indicate that the pin or pad
 * overlaps a polygon on some layer.
 * 0x0010 clearpoly
 * For polygons, this flag means that pins and vias will normally clear these polygons
 * (thus, thermals are required for electrical connection). When clear, polygons
 * will solidly connect to pins and vias.
 * 0x0010 hidename
 * For elements, when set the name of the element is hidden.
 * 0x0020 showname
 * For elements, when set the names of pins are shown.
 * 0x0020 clearline
 * For lines and arcs, the line/arc will clear polygons instead of connecting to
 * them.
 * 0x0020 fullpoly
 * For polygons, the full polygon is drawn (i.e. all parts instead of only the biggest
 * one).
 * 0x0040 selected
 * Set when the object is selected.
 * 0x0080 onsolder
 * For elements and pads, indicates that they are on the solder side
 * 0x0080 auto
 * For lines and vias, indicates that these were created by the autorouter.
 * 0x0100 square
 * For pins and pads, indicates a square (vs round) pin/pad.
 * 0x0200 rubberend
 * For lines, used internally for rubber band moves.
 * 0x0200 warn
 * For pins, vias, and pads, set to indicate a warning.
 * 0x0400 usetherm
 * Obsolete, indicates that pins/vias should be drawn with thermal fingers.
 * 0x0400 Obsolete, old files used this to indicate lines drawn on silk.
 * 0x0800 octagon
 * Draw pins and vias as octagons.
 * 0x1000 drc
 * Set for objects that fail DRC.
 * 0x2000 lock
 * Set for locked objects.
 * 0x4000 edge2
 * For pads, indicates that the second point is closer to the edge. For pins, indicates
 * that the pin is closer to a horizontal edge and thus pinout text should be vertical.
 * 0x8000 marker
 * Marker used internally to avoid revisiting an object.
 * 0x10000 nopaste
 * For pads, set to prevent a solderpaste stencil opening for the pad. Primarily
 * used for pads used as fiducials.
 */
bool MODULE::Read_GPCB_Descr( const wxString& CmpFullFileName )
{
    #define TEXT_DEFAULT_SIZE  400
    #define OLD_GPCB_UNIT_CONV 10
    #define NEW_GPCB_UNIT_CONV 0.1

    FILE*         cmpfile;
    double        conv_unit = NEW_GPCB_UNIT_CONV; // GPCB unit = 0.01 mils and Pcbnew 0.1

    // Old version unit = 1 mil, so conv_unit is 10 or 0.1
    bool          success = true;
    char*         line;
    long          ibuf[100];
    EDGE_MODULE*  drawSeg;
    D_PAD*        pad;
    wxArrayString params;
    int           iprmcnt, icnt_max, iflgidx;

    if( ( cmpfile = wxFopen( CmpFullFileName, wxT( "rt" ) ) ) == NULL )
        return false;

    FILE_LINE_READER fileReader( cmpfile, CmpFullFileName );

    FILTER_READER reader( fileReader );

    reader.ReadLine();

    line = reader.Line();

    params.Clear();
    Extract_Parameters( params, line );

    iprmcnt  = 0;
    icnt_max = params.GetCount();

    if( params[iprmcnt].CmpNoCase( wxT( "Element" ) ) != 0 )
    {
        return false;
    }

    // Test symbol after "Element": if [ units = 0.01 mils, and if ( units = 1 mil
    iprmcnt++;

    if( params[iprmcnt] == wxT( "(" ) )
        conv_unit = OLD_GPCB_UNIT_CONV;

    /* Analyse first line :
     * Element [element_flags, description, pcb-name, value, mark_x, mark_y, text_x, text_y,
     *      text_direction, text_scale, text_flags]
     */

    // Read flags (unused)
    iprmcnt++;

    // Read description
    iprmcnt++;
    m_Doc = params[iprmcnt];

    // Read pcb-name (reference )
    iprmcnt++;
    m_Reference->m_Text = params[iprmcnt];

    // Read value
    iprmcnt++;
    m_Value->m_Text = params[iprmcnt];

    // Read other infos
    int idx = 2;    // index of the first param of the ref text in ibuf
                    // can be 2 ( 0 and 1 = position of module (not handled by Pcbnew)
                    // or 0 if no module position
    iprmcnt++;
    for( int ii = 0; ii < 20; ii++ )
        ibuf[ii] = 0;

    for( int ii = 0; ii <= 8; ii++, iprmcnt++ ) // upt to 6 params + terminal char.
    {
        if( iprmcnt >= icnt_max )
        {
            success  = false;
            break;
        }
        else
        {
            if( params[iprmcnt] == wxT( ")" ) ||
                params[iprmcnt] == wxT( "]" ) )
            {   // Terminal character found
                if( ii <= 5 ) // no module position
                    idx = 0;
                break;
            }
            params[iprmcnt].ToLong( &ibuf[ii] );
        }
    }
    wxPoint pos;
    pos.x  = wxRound( ibuf[idx] * conv_unit );
    pos.y  = wxRound( ibuf[idx+1] * conv_unit );
    m_Reference->SetPos( pos );
    m_Reference->SetPos0( pos );
    m_Reference->SetOrientation( ibuf[idx+2] ? 900 : 0 );

    // Calculate size: default is 40 mils (400 pcb units)
    // real size is:  default * ibuf[idx+3] / 100 (size in gpcb is given in percent of default size
    int tsize = ( ibuf[idx+3] * TEXT_DEFAULT_SIZE ) / 100;
    int thickness = m_Reference->m_Size.x / 6;

    tsize = MAX( 40, tsize );
    m_Reference->SetSize( wxSize( tsize, tsize ) );
    m_Reference->m_Thickness  = thickness;
    m_Value->SetOrientation( m_Reference->GetOrientation() );
    m_Value->SetSize( m_Reference->GetSize() );
    m_Value->m_Thickness  = m_Reference->m_Thickness;
    pos.y += tsize + thickness;
    m_Value->SetPos( pos );
    m_Value->SetPos0( pos );

    while( reader.ReadLine() )
    {
        line = reader.Line();
        params.Clear();
        Extract_Parameters( params, line );

        if( params.GetCount() > 3 )    // Test units value for a string line param (more than 3 params : ident [ xx ] )
        {
            if( params[1] == wxT( "(" ) )
                conv_unit = OLD_GPCB_UNIT_CONV;
            else
                conv_unit = NEW_GPCB_UNIT_CONV;
        }

        if( params[0].CmpNoCase( wxT( "ElementLine" ) ) == 0 )      // line descr
        {                                                           // Format: ElementLine [X1 Y1 X2 Y2 Thickness]
            wxPoint start0;
            wxPoint end0;
            int     width;

            int*    list[5] = {
                        &start0.x, &start0.y,
                        &end0.x,   &end0.y,
                        &width
                    };

            for( unsigned ii = 0; ii < 5; ii++ )
            {
                long dim;

                if( ii < (params.GetCount() - 2) )
                {
                    if( params[ii + 2].ToLong( &dim ) )
                        *list[ii] = wxRound( dim * conv_unit );
                }
            }

            drawSeg = new EDGE_MODULE( this );
            drawSeg->SetLayer( SILKSCREEN_N_FRONT );
            drawSeg->SetShape( S_SEGMENT );

            drawSeg->SetStart0( start0 );
            drawSeg->SetEnd0( end0 );
            drawSeg->SetWidth( width );

            drawSeg->SetDrawCoord();

            m_Drawings.PushBack( drawSeg );

            continue;
        }

        if( params[0].CmpNoCase( wxT( "ElementArc" ) ) == 0 )       // Arc descr
        {                                                           // format: ElementArc [X Y Width Height StartAngle DeltaAngle Thickness]
            // Pcbnew does know ellipse so we must have Width = Height
            drawSeg = new EDGE_MODULE( this );
            drawSeg->SetLayer( SILKSCREEN_N_FRONT );
            drawSeg->SetShape( S_ARC );

            m_Drawings.PushBack( drawSeg );

            for( unsigned ii = 0; ii < 7; ii++ )
            {
                long dim;
                if( ii < (params.GetCount() - 2) )
                {
                    if( params[ii + 2].ToLong( &dim ) )
                        ibuf[ii] = dim;
                    else
                        ibuf[ii] = 0;
                }
                else
                    ibuf[ii] = 0;
            }

            // for and arc: ibuf[3] = ibuf[4]. Pcbnew does not know ellipses
            int     radius = (ibuf[2] + ibuf[3]) / 4;

            wxPoint centre;
            centre.x = wxRound( ibuf[0] * conv_unit );
            centre.y = wxRound( ibuf[1] * conv_unit );

            drawSeg->SetStart0( centre );

            double start_angle = ibuf[4] * 10;  // Pcbnew uses 0.1 degrees as units
            start_angle       -= 1800;          // Use normal X axis  as reference

            drawSeg->SetAngle( ibuf[5] * 10 ); // Angle value is clockwise in gpcb and Pcbnew

            drawSeg->SetEnd0( wxPoint( wxRound( radius * conv_unit ), 0 ) );

            // Calculate start point coordinate of arc
            wxPoint arcStart( drawSeg->GetEnd0() );
            RotatePoint( &arcStart, -start_angle );
            drawSeg->SetEnd0( centre + arcStart );

            drawSeg->SetWidth( wxRound( ibuf[6] * conv_unit ) );
            drawSeg->SetDrawCoord();
            continue;
        }

        if( params[0].CmpNoCase( wxT( "Pad" ) ) == 0 )      // Pad with no hole (smd pad)
        {
            //  format: Pad [x1 y1 x2 y2 thickness clearance mask "name" "pad_number" flags]
            pad = new D_PAD( this );
            pad->SetShape( PAD_RECT );
            pad->SetLayerMask( LAYER_FRONT | SOLDERMASK_LAYER_FRONT | SOLDERPASTE_LAYER_FRONT );

            // Set shape from flags
            iflgidx = params.GetCount() - 2;

            if( TestFlags( params[iflgidx], 0x0080, wxT( "onsolder" ) ) )
                pad->SetLayerMask( LAYER_BACK | SOLDERMASK_LAYER_BACK | SOLDERPASTE_LAYER_BACK );

            for( unsigned ii = 0; ii < 5; ii++ )
            {
                if( ii < params.GetCount() - 2 )
                {
                    long dim;

                    if( params[ii + 2].ToLong( &dim ) )
                        ibuf[ii] = wxRound( dim * conv_unit );
                }
                else
                {
                    ibuf[ii] = 0;
                }
            }

            // Read name:
            // Currently unused

            // Read pad number:
            if( params[1] == wxT( "(" ) )
            {
                pad->SetPadName( params[8] );
            }
            else
            {
                pad->SetPadName( params[10] );
            }
            // Calculate the Pad parameters.
            // In Pcb the shape is a segment
            // ibuf[0], ibuf[1] is the start point of the segment
            // ibuf[2], ibuf[3] is the end point of the segment
            // and me must convert the segment to an oval ( or rectangular) pad
            // Pad pos = middle of the segment
            // Pad Orientation = angle of the segment
            // Pad size = lenght and thickness of the segment
            wxPoint delta;
            delta.x = ibuf[2] - ibuf[0];
            delta.y = ibuf[3] - ibuf[1];

            double angle = atan2( (double)delta.y, (double)delta.x );

            // Negate angle (due to Y reversed axis) and convert it to internal units
            angle = - angle * 1800.0 / M_PI;
            pad->SetOrientation( wxRound( angle ) );

            wxPoint padPos( (ibuf[0] + ibuf[2]) / 2, (ibuf[1] + ibuf[3]) / 2 );

            pad->SetSize( wxSize(
                    wxRound( hypot( (double)delta.x, (double)delta.y ) ) + ibuf[4],
                    ibuf[4] ) );

            padPos += m_Pos;
            pad->SetPos0( padPos );
            pad->SetPosition( padPos );

            if( !TestFlags( params[iflgidx], 0x0100, wxT( "square" ) ) )
            {
                if( pad->GetSize().x == pad->GetSize().y )
                    pad->SetShape( PAD_ROUND );
                else
                    pad->SetShape( PAD_OVAL );
            }

            m_Pads.PushBack( pad );
            continue;
        }

        if( params[0].CmpNoCase( wxT( "Pin" ) ) == 0 )      // Pad with hole (trough pad)
        {
            // format: Pin[x y Thickness Clearance Mask DrillHole Name Number Flags]
            pad = new D_PAD( this );
            pad->SetShape( PAD_ROUND );

            pad->SetLayerMask(    ALL_CU_LAYERS |
                                  SILKSCREEN_LAYER_FRONT |
                                  SOLDERMASK_LAYER_FRONT |
                                  SOLDERMASK_LAYER_BACK );

            iflgidx = params.GetCount() - 2;

            if( TestFlags( params[iflgidx], 0x0100, wxT( "square" ) ) )
                pad->SetShape( PAD_RECT );

            for( unsigned ii = 0; ii < 6; ii++ )
            {
                if( ii < params.GetCount() - 2 )
                {
                    long dim;
                    if( params[ii + 2].ToLong( &dim ) )
                        ibuf[ii] = wxRound( dim * conv_unit );
                }
                else
                {
                    ibuf[ii] = 0;
                }
            }

            // Read name:
            // Currently unused

            // Read pad number:
            if( params[1] == wxT( "(" ) )
            {
                pad->SetPadName( params[7] );
            }
            else
            {
                pad->SetPadName( params[9] );
            }

            wxPoint padPos( ibuf[0], ibuf[1] );

            pad->SetDrillSize( wxSize( ibuf[5], ibuf[5] ) );

            int sz = ibuf[3] + pad->GetDrillSize().x;

            pad->SetSize( wxSize( sz, sz ) );

            padPos += m_Pos;

            pad->SetPos0( padPos );
            pad->SetPosition( padPos );

            if( pad->GetShape() == PAD_ROUND  &&  pad->GetSize().x != pad->GetSize().y )
                pad->SetShape( PAD_OVAL );

            m_Pads.PushBack( pad );
            continue;
        }
    }

    if( m_Value->m_Text.IsEmpty() )
        m_Value->m_Text = wxT( "Val**" );

    if( m_Reference->m_Text.IsEmpty() )
    {
        wxFileName filename( CmpFullFileName );
        m_Reference->m_Text = filename.GetName();
    }

    // Recalculate the bounding box
    CalculateBoundingBox();
    return success;
}


/* Read a text line and extract params and tokens.
 * special chars are:
 * [ ] ( )  Begin and end of parameter list and units indicator
 * " is a string delimiter
 * space is the param separator
 * The first word is the keyword
 * the second item is one of ( ot [
 * other are parameters (number or delimited string)
 * last parameter is ) or ]
 */
static void Extract_Parameters( wxArrayString& param_list, char* text )
{
    char     key;
    wxString tmp;

    while( *text != 0 )
    {
        key = *text;
        text++;

        switch( key )
        {
        case '[':
        case ']':
        case '(':
        case ')':
            if( !tmp.IsEmpty() )
            {
                param_list.Add( tmp );
                tmp.Clear();
            }
            tmp.Append( key );
            param_list.Add( tmp );
            tmp.Clear();
            break;

        case '\n':
        case '\r':
        case '\t':
        case ' ':
            if( !tmp.IsEmpty() )
            {
                param_list.Add( tmp );
                tmp.Clear();
            }
            break;

        case '"':
            while( *text != 0 )
            {
                key = *text;
                text++;

                if( key == '"' )
                {
                    param_list.Add( tmp );
                    tmp.Clear();
                    break;
                }
                else
                {
                    tmp.Append( key );
                }
            }

            break;

        default:
            tmp.Append( key );
            break;
        }
    }
}


/**
 * Function TestFlags
 * Test flag flg_mask or flg_name.
 * @param flg_string = flsg list to test: can be a bit field flag or a list name flsg
 * a bit field flag is an hexadecimal value: Ox00020000
 * a list name flag is a string list of flags, comma separated like square,option1
 * @param flg_mask = flag list to test
 * @param flg_name = flag name to find in list
 * @return true if found
 */
bool TestFlags( const wxString& flg_string, long flg_mask, const wxChar* flg_name )
{
    wxString strnumber;

    if( flg_string.StartsWith( wxT( "0x" ), &strnumber )
        || flg_string.StartsWith( wxT( "0X" ), &strnumber ) )
    {
        long lflags;

        if( strnumber.ToLong( &lflags, 16 ) )
            if( lflags & flg_mask )
                return true;
    }
    else if( flg_string.Contains( flg_name ) )
    {
        return true;
    }

    return false;
}
