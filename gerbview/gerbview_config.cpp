/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2007 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2009 Wayne Stambaugh <stambaughw@verizon.net>
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
 * @file gerbview_config.cpp
 * @brief GerbView configuration.
*/

#include <fctsys.h>
#include <macros.h>
#include <id.h>
#include <common.h>
#include <class_drawpanel.h>
#include <gestfich.h>
#include <pcbcommon.h>
#include <param_config.h>
#include <colors_selection.h>

#include <gerbview.h>
#include <hotkeys.h>
#include <class_board_design_settings.h>
#include <dialog_hotkeys_editor.h>


#define GROUP wxT("/gerbview")

#define INSETUP true


void GERBVIEW_FRAME::Process_Config( wxCommandEvent& event )
{
    int      id = event.GetId();
    wxString FullFileName;

    switch( id )
    {
    // Hotkey IDs
    case ID_PREFERENCES_HOTKEY_EXPORT_CONFIG:
        ExportHotkeyConfigToFile( s_Gerbview_Hokeys_Descr );
        break;

    case ID_PREFERENCES_HOTKEY_IMPORT_CONFIG:
        ImportHotkeyConfigFromFile( s_Gerbview_Hokeys_Descr );
        break;

    case ID_PREFERENCES_HOTKEY_SHOW_EDITOR:
        InstallHotkeyFrame( this, s_Gerbview_Hokeys_Descr );
        break;

    case ID_PREFERENCES_HOTKEY_SHOW_CURRENT_LIST:

        // Display current hotkey list for GerbView.
        DisplayHotkeyList( this, s_Gerbview_Hokeys_Descr );
        break;

    default:
        wxMessageBox( wxT( "GERBVIEW_FRAME::Process_Config error" ) );
        break;
    }
}


PARAM_CFG_ARRAY& GERBVIEW_FRAME::GetConfigurationSettings()
{
    if( !m_configSettings.empty() )
        return m_configSettings;

    m_configSettings.push_back( new PARAM_CFG_INT( true, wxT( "Units" ),
                                                   (int*) &g_UserUnit, 0, 0, 1 ) );

    m_configSettings.push_back( new PARAM_CFG_INT( true, wxT( "DrawModeOption" ),
                                                   &m_displayMode, 2, 0, 2 ) );
    m_configSettings.push_back( new PARAM_CFG_SETCOLOR( true,
                                                        wxT( "DCodeColor" ),
                                                        &g_ColorsSettings.m_ItemsColors[
                                                            DCODES_VISIBLE],
                                                        WHITE ) );

    m_configSettings.push_back( new PARAM_CFG_BOOL( true,
                                                    wxT( "DisplayPolarCoordinates" ),
                                                    &DisplayOpt.DisplayPolarCood,
                                                    false ) );

    // Default colors for layers 0 to 31
    static const int color_default[] = {
        GREEN,     BLUE,         LIGHTGRAY, MAGENTA,
        RED,       DARKGREEN,    BROWN,     MAGENTA,
        LIGHTGRAY, BLUE,         GREEN,     CYAN,
        LIGHTRED,  LIGHTMAGENTA, YELLOW,    RED,
        BLUE,      BROWN,        LIGHTCYAN, RED,
        MAGENTA,   CYAN,         BROWN,     MAGENTA,
        LIGHTGRAY, BLUE,         GREEN,     DARKCYAN,
        YELLOW,    LIGHTMAGENTA, YELLOW,    LIGHTGRAY,
    };

    // List of keywords used as identifiers in config.
    // They *must* be static const and not temporarily created,
    // because the parameter list that use these keywords does not store them,
    // just points to them.
    static const wxChar* keys[] = {
        wxT("ColorLayer0"),     wxT("ColorLayer1"),     wxT("ColorLayer2"),     wxT("ColorLayer3"),
        wxT("ColorLayer4"),     wxT("ColorLayer5"),     wxT("ColorLayer6"),     wxT("ColorLayer7"),
        wxT("ColorLayer8"),     wxT("ColorLayer9"),     wxT("ColorLayer10"),    wxT("ColorLayer11"),
        wxT("ColorLayer12"),    wxT("ColorLayer13"),    wxT("ColorLayer14"),    wxT("ColorLayer15"),

        wxT("ColorLayer16"),    wxT("ColorLayer17"),    wxT("ColorLayer18"),    wxT("ColorLayer19"),
        wxT("ColorLayer20"),    wxT("ColorLayer21"),    wxT("ColorLayer22"),    wxT("ColorLayer23"),
        wxT("ColorLayer24"),    wxT("ColorLayer25"),    wxT("ColorLayer26"),    wxT("ColorLayer27"),
        wxT("ColorLayer28"),    wxT("ColorLayer29"),    wxT("ColorLayer30"),    wxT("ColorLayer31"),
    };

    wxASSERT( DIM(keys) == DIM(color_default) );
    wxASSERT( DIM(keys) <= DIM(g_ColorsSettings.m_LayersColors) && DIM(keys) <= DIM(color_default) );

    for( unsigned i = 0; i < DIM(keys);  ++i )
    {
        int* prm = &g_ColorsSettings.m_LayersColors[i];

        PARAM_CFG_SETCOLOR* prm_entry =
            new PARAM_CFG_SETCOLOR( true, keys[i], prm, color_default[i] );

        m_configSettings.push_back( prm_entry );
    }

    return m_configSettings;
}
