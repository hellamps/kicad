/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2007-2011 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2008-2011 Wayne Stambaugh <stambaughw@verizon.net>
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
 * The common library
 * @file common.h
 */

#ifndef INCLUDE__COMMON_H_
#define INCLUDE__COMMON_H_

#include <vector>

#include <wx/wx.h>
#include <wx/confbase.h>
#include <wx/fileconf.h>


class wxAboutDialogInfo;

// Flag for special keys
#define GR_KB_RIGHTSHIFT 0x10000000                 /* Keybd states: right
                                                     * shift key depressed */
#define GR_KB_LEFTSHIFT  0x20000000                 /* left shift key depressed
                                                     */
#define GR_KB_CTRL       0x40000000                 // CTRL depressed
#define GR_KB_ALT        0x80000000                 // ALT depressed
#define GR_KB_SHIFT      (GR_KB_LEFTSHIFT | GR_KB_RIGHTSHIFT)
#define GR_KB_SHIFTCTRL  (GR_KB_SHIFT | GR_KB_CTRL)
#define MOUSE_MIDDLE     0x08000000                 /* Middle button mouse
                                                     * flag for block commands
                                                     */

/// default name for nameless projects
#define NAMELESS_PROJECT wxT( "noname" )


/// Pseudo key codes for command panning
enum pseudokeys {
    EDA_PANNING_UP_KEY = 1,
    EDA_PANNING_DOWN_KEY,
    EDA_PANNING_LEFT_KEY,
    EDA_PANNING_RIGHT_KEY,
    EDA_ZOOM_IN_FROM_MOUSE,
    EDA_ZOOM_OUT_FROM_MOUSE,
    EDA_ZOOM_CENTER_FROM_MOUSE
};

#define ESC 27

// TODO Executable names TODO
#ifdef __WINDOWS__
#define CVPCB_EXE           wxT( "cvpcb.exe" )
#define PCBNEW_EXE          wxT( "pcbnew.exe" )
#define EESCHEMA_EXE        wxT( "eeschema.exe" )
#define GERBVIEW_EXE        wxT( "gerbview.exe" )
#define BITMAPCONVERTER_EXE wxT( "bitmap2component.exe" )
#define PCB_CALCULATOR_EXE  wxT( "pcb_calculator.exe" )
#else
#ifndef __WXMAC__
#define CVPCB_EXE           wxT( "cvpcb" )
#define PCBNEW_EXE          wxT( "pcbnew" )
#define EESCHEMA_EXE        wxT( "eeschema" )
#define GERBVIEW_EXE        wxT( "gerbview" )
#define BITMAPCONVERTER_EXE wxT( "bitmap2component" )
#define PCB_CALCULATOR_EXE  wxT( "pcb_calculator" )
#else
#define CVPCB_EXE           wxT( "cvpcb.app/Contents/MacOS/cvpcb" )
#define PCBNEW_EXE          wxT( "pcbnew.app/Contents/MacOS/pcbnew" )
#define EESCHEMA_EXE        wxT( "eeschema.app/Contents/MacOS/eeschema" )
#define GERBVIEW_EXE        wxT( "gerbview.app/Contents/MacOS/gerbview" )
#define BITMAPCONVERTER_EXE wxT( "bitmap2component.app/Contents/MacOS/bitmap2component" )
#define PCB_CALCULATOR_EXE  wxT( "pcb_calculator.app/Contents/MacOS/pcb_calculator" )
# endif
#endif


// Graphic Texts Orientation in 0.1 degree
#define TEXT_ORIENT_HORIZ 0
#define TEXT_ORIENT_VERT  900

#define ON  1
#define OFF 0


enum EDA_UNITS_T {
    INCHES = 0,
    MILLIMETRES = 1,
    UNSCALED_UNITS = 2
};


// forward declarations:
class LibNameList;


/**
 * Class PAGE_INFO
 * describes the page size and margins of a paper page on which to
 * eventually print or plot.  Paper sizes are often described in inches.
 * Here paper is described in 1/1000th of an inch (mils).  For convenience
 * there are some read only accessors for internal units (IU), which is a compile
 * time calculation, not runtime.
 *
 * @author Dick Hollenbeck
 */
class PAGE_INFO
{
public:

    static const wxString Custom;     /// "User" defined page type

    PAGE_INFO( const wxString& aType = wxT( "A3" ) );
    PAGE_INFO( const wxSize& aSizeMils, const wxString& aName );

    /**
     * Function SetType
     * sets the name of the page type and also the sizes and margins
     * commonly associated with that type name.
     *
     * @param aStandardPageDescriptionName is a wxString constant giving one of:
     * "A4" "A3" "A2" "A1" "A0" "A" "B" "C" "D" "E" "GERBER", "USLetter", "USLegal",
     * "USLedger", or "User".  If "User" then the width and height are custom,
     * and will be set according to <b>previous</b> calls to
     * static PAGE_INFO::SetUserWidthMils() and
     * static PAGE_INFO::SetUserHeightMils();
     *
     * @return bool - true iff @a aStandarePageDescription was a recognized type.
     */
    bool SetType( const wxString& aStandardPageDescriptionName );
    const wxString& GetType() const { return m_type; }

    /**
     * Function IsCustom
     * returns true if the type is Custom
     */
    bool IsCustom() const;

    /**
     * Function SetPortrait
     * will rotate the paper page 90 degrees.  This PAGE_INFO may either be in
     * portrait or landscape mode.  Use this function to change from one to the
     * other mode.
     * @param isPortrait if true and not already in portrait mode, will change
     *  this PAGE_INFO to portrait mode.  Or if false and not already in landscape mode,
     *  will change this PAGE_INFO to landscape mode.
     */
    void SetPortrait( bool isPortrait );
    bool IsPortrait() const { return m_portrait; }

    void SetWidthMils(  int aWidthInMils );
    int GetWidthMils() const { return m_size.x; }

    void SetHeightMils( int aHeightInMils );
    int GetHeightMils() const { return m_size.y; }

    const wxSize& GetSizeMils() const { return m_size; }

    // Accessors returning "Internal Units (IU)".  IUs are mils in EESCHEMA,
    // and either deci-mils or nanometers in PCBNew.
#if defined(PCBNEW)
# if defined(KICAD_NANOMETRE)
    int GetWidthIU() const  { return 25400 * GetWidthMils();  }
    int GetHeightIU() const { return 25400 * GetHeightMils(); }
# else
    int GetWidthIU() const  { return 10 * GetWidthMils();  }
    int GetHeightIU() const { return 10 * GetHeightMils(); }
# endif
    const wxSize GetSizeIU() const  { return wxSize( GetWidthIU(), GetHeightIU() ); }
#elif defined(EESCHEMA)
    int GetWidthIU() const  { return GetWidthMils();  }
    int GetHeightIU() const { return GetHeightMils(); }
    const wxSize GetSizeIU() const  { return wxSize( GetWidthIU(), GetHeightIU() ); }
#endif

    int GetLeftMarginMils() const           { return m_left_margin; }
    int GetRightMarginMils() const          { return m_right_margin; }
    int GetTopMarginMils() const            { return m_top_margin; }
    int GetBottomMarginMils() const         { return m_bottom_margin; }

    void SetLeftMarginMils( int aMargin )   { m_left_margin = aMargin; }
    void SetRightMarginMils( int aMargin )  { m_right_margin = aMargin; }
    void SetTopMarginMils( int aMargin )    { m_top_margin = aMargin; }
    void SetBottomMarginMils( int aMargin ) { m_bottom_margin = aMargin; }

    /**
     * Function SetUserWidthMils
     * sets the width of type "User" page in mils, for any type "User" page
     * constructed or made via SetType() after making this call.
     */
    static void SetUserWidthMils( int aWidthInMils );

    /**
     * Function SetUserHeightMils
     * sets the height type "User" page in mils, for any type "User" page
     * constructed or made via SetType() after making this call.
     */
    static void SetUserHeightMils( int aHeightInMils );

    /**
     * Function GetStandardSizes
     * returns the standard page types, such as "A4", "A3", etc.
    static wxArrayString GetStandardSizes();
     */

private:

    // standard pre-defined sizes
    static const PAGE_INFO pageA4;
    static const PAGE_INFO pageA3;
    static const PAGE_INFO pageA2;
    static const PAGE_INFO pageA1;
    static const PAGE_INFO pageA0;
    static const PAGE_INFO pageA;
    static const PAGE_INFO pageB;
    static const PAGE_INFO pageC;
    static const PAGE_INFO pageD;
    static const PAGE_INFO pageE;
    static const PAGE_INFO pageGERBER;

    static const PAGE_INFO pageUSLetter;
    static const PAGE_INFO pageUSLegal;
    static const PAGE_INFO pageUSLedger;

    static const PAGE_INFO pageUser;

    // all dimensions here are in mils

    wxString    m_type;             ///< paper type: A4, A3, etc.
    wxSize      m_size;             ///< mils

    int         m_left_margin;
    int         m_right_margin;
    int         m_top_margin;
    int         m_bottom_margin;

    bool        m_portrait;         ///< true if portrait, false if landscape

    static int s_user_height;
    static int s_user_width;

    void updatePortrait();
};


extern wxString     g_ProductName;

/// Default user lib path can be left void, if the standard lib path is used
extern wxString     g_UserLibDirBuffer;

extern bool         g_ShowPageLimits;       ///< true to display the page limits

/// Name of default configuration file. (kicad.pro)
extern wxString     g_Prj_Default_Config_FullFilename;

/// Name of local configuration file. (\<curr projet\>.pro)
extern wxString     g_Prj_Config_LocalFilename;

extern EDA_UNITS_T  g_UserUnit;     ///< display units

/// Draw color for moving objects.
extern int          g_GhostColor;


// COMMON.CPP

/**
 * Function SetLocaleTo_C_standard
 *  because KiCad is internationalized, switch internalization to "C" standard
 *  i.e. uses the . (dot) as separator in print/read float numbers
 *  (some countries (France, Germany ..) use , (comma) as separator)
 *  This function must be called before read or write ascii files using float
 *  numbers in data the SetLocaleTo_C_standard function must be called after
 *  reading or writing the file
 *
 *  This is wrapper to the C setlocale( LC_NUMERIC, "C" ) function,
 *  but could make more easier an optional use of locale in KiCad
 */
void SetLocaleTo_C_standard();

/**
 * Function SetLocaleTo_Default
 *  because KiCad is internationalized, switch internalization to default
 *  to use the default separator in print/read float numbers
 *  (. (dot) but some countries (France, Germany ..) use , (comma) as
 *   separator)
 *  This function must be called after a call to SetLocaleTo_C_standard
 *
 *  This is wrapper to the C setlocale( LC_NUMERIC, "" ) function,
 *  but could make more easier an optional use of locale in KiCad
 */
void SetLocaleTo_Default();

/**
 * Class LOCALE_IO
 * is a class that can be instantiated within a scope in which you are expecting
 * exceptions to be thrown.  Its constructor calls SetLocaleTo_C_Standard().
 * Its destructor insures that the default locale is restored if an exception
 * is thrown, or not.
 */
class LOCALE_IO
{
public:
    LOCALE_IO()     { SetLocaleTo_C_standard(); }
    ~LOCALE_IO()    { SetLocaleTo_Default(); }
};

/**
 * Function EnsureTextCtrlWidth
 * sets the minimum pixel width on a text control in order to make a text
 * string be fully visible within it. The current font within the text
 * control is considered.
 * The text can come either from the control or be given as an argument.
 * If the text control is larger than needed, then nothing is done.
 * @param aCtrl the text control to potentially make wider.
 * @param aString the text that is used in sizing the control's pixel width.
 * If NULL, then
 *   the text already within the control is used.
 * @return bool - true if the \a aCtrl had its size changed, else false.
 */
bool EnsureTextCtrlWidth( wxTextCtrl* aCtrl, const wxString* aString = NULL );


/**
 * Operator << overload
 * outputs a point to the argument string in a format resembling
 * "@ (x,y)
 * @param aString Where to put the text describing the point value
 * @param aPoint  The point to output.
 * @return wxString& - the input string
 */
wxString& operator <<( wxString& aString, const wxPoint& aPoint );


/**
 * Function ProcessExecute
 * runs a child process.
 * @param aCommandLine The process and any arguments to it all in a single
 *                     string.
 * @param aFlags The same args as allowed for wxExecute()
 * @return bool - true if success, else false
 */
bool ProcessExecute( const wxString& aCommandLine, int aFlags = wxEXEC_ASYNC );


/*******************/
/* about_kicad.cpp */
/*******************/
void InitKiCadAbout( wxAboutDialogInfo& info );


/**************/
/* common.cpp */
/**************/

/**
 * @return an unique time stamp that changes after each call
 */
unsigned long GetNewTimeStamp();

int DisplayColorFrame( wxWindow* parent, int OldColor );
int GetCommandOptions( const int argc, const char** argv,
                       const char* stringtst, const char** optarg,
                       int* optind );


/* Returns to display the value of a parameter, by type of units selected
 * Input: value in mils, buffer text
 * Returns to buffer: text: value expressed in inches or millimeters
 * Followed by " or mm
 */
const wxString& valeur_param( int valeur, wxString& buf_texte );

/**
 * Function CoordinateToString
 * is a helper to convert the integer coordinate \a aValue to a string in inches,
 * millimeters, or unscaled units according to the current user units setting.
 *
 * @param aValue The coordinate to convert.
 * @param aInternalUnits The internal units of the application.  #EESCHEMA_INTERNAL_UNIT
 *                       and #PCB_INTERNAL_UNIT are the only valid value.
 * @param aConvertToMils Convert inch values to mils if true.  This setting has no effect if
 *                       the current user unit is millimeters.
 * @return The converted string for display in user interface elements.
 */
wxString CoordinateToString( int aValue, int aInternalUnits, bool aConvertToMils = false );

/**
 * Returns the units symbol.
 *
 * @param aUnits - Units type, default is current units setting.
 * @param aFormatString - A formatting string to embed the units symbol into.  Note:
 *                        the format string must contain the %s format specifier.
 * @return The formatted units symbol.
 */
wxString ReturnUnitSymbol( EDA_UNITS_T aUnits = g_UserUnit,
                           const wxString& aFormatString = _( " (%s):" ) );

/**
 * Get a human readable units string.
 *
 * The strings returned are full text name and not abbreviations or symbolic
 * representations of the units.  Use ReturnUnitSymbol() for that.
 *
 * @param aUnits - The units text to return.
 * @return The human readable units string.
 */
wxString GetUnitsLabel( EDA_UNITS_T aUnits );
wxString GetAbbreviatedUnitsLabel( EDA_UNITS_T aUnit = g_UserUnit );

/**
 * Function ReturnValueFromeString
 * @return The string from Value, according to units (inch, mm ...) for display,
 *  and the initial unit for value
 *  Unit = display units (INCH, MM ..)
 *  Value = text
 *  Internal_Unit = units per inch for computed value
 */
int ReturnValueFromString( EDA_UNITS_T aUnit, const wxString& TextValue, int Internal_Unit );

/**
 * Function ReturnStringFromValue
 * Return the string from Value, according to units (inch, mm ...) for display,
 * and the initial unit for value
 * @param aUnit = display units (INCHES, MILLIMETRE ..)
 * @param aValue = value in Internal_Unit
 * @param aInternal_Unit = units per inch for Value
 * @param aAdd_unit_symbol = true to add symbol unit to the string value
 * @return a wxString what contains value and optionally the symbol unit (like
 *         2.000 mm)
 */
wxString        ReturnStringFromValue( EDA_UNITS_T aUnit,
                                       int  aValue,
                                       int  aInternal_Unit,
                                       bool aAdd_unit_symbol = false );

void            AddUnitSymbol( wxStaticText& Stext, EDA_UNITS_T aUnit = g_UserUnit );

/* Add string "  (mm):" or " ("):" to the static text Stext.
 *  Used in dialog boxes for entering values depending on selected units */
void            PutValueInLocalUnits( wxTextCtrl& TextCtr, int Value,
                                      int Internal_Unit );

/* Convert the number Value in a string according to the internal units
 *  and the selected unit (g_UserUnit) and put it in the wxTextCtrl TextCtrl
 **/
int             ReturnValueFromTextCtrl( const wxTextCtrl& TextCtr,
                                         int               Internal_Unit );

/**
 * Function wxStringSplit
 * splits \a aString to a string list separated at \a aSplitter.
 * @return the list
 * @param aString is the text to split
 * @param aSplitter is the 'split' character
 */
wxArrayString* wxStringSplit( wxString aString, wxChar aSplitter );

/**
 * Function To_User_Unit
 * Convert in inch or mm the variable "val" (double)given in internal units
 * @return the converted value, in double
 * @param aUnit : user unit to be converted to
 * @param val : double : the given value

 * @param internal_unit_value = internal units per inch
 */
double To_User_Unit( EDA_UNITS_T aUnit, double val, int internal_unit_value );

/**
 * Return in internal units the value "val" given in inch or mm
 */
int From_User_Unit( EDA_UNITS_T aUnit, double val, int internal_unit_value );

/**
 * Function GenDate
 * @return A wsString object containg the date in the format "day month year" like
 *         "23 jun 2005".
 */
wxString GenDate();

/**
 * Function GetRunningMicroSecs
 * returns an ever increasing indication of elapsed microseconds.  Use this
 * by computing differences between two calls.
 * @author Dick Hollenbeck
 */
unsigned GetRunningMicroSecs();

#endif  // INCLUDE__COMMON_H_
