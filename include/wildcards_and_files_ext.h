/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 20012 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2007-2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2008-2012 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 1992-2012 KiCad Developers, see AUTHORS.txt for contributors.
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
 * @file wildcards_and_files_ext.h
 */

#ifndef INCLUDE_WILDCARDS_AND_FILES_EXT_H_
#define INCLUDE_WILDCARDS_AND_FILES_EXT_H_

#include <wx/wx.h>

/**
 * File extension definitions.  Please do not changes these.  If a different
 * file extension is needed, create a new definition in the application.
 * Please note, just because they are defined as const doesn't guarantee
 * that they cannot be changed.
 * Mainly wild cards are most of time translated when displayed
 */
extern const wxString VrmlFileExtension;
extern const wxString ProjectFileExtension;
extern const wxString SchematicFileExtension;
extern const wxString NetlistFileExtension;
extern const wxString GerberFileExtension;
extern const wxString PcbFileExtension;
extern const wxString FootprintLibFileExtension;
extern const wxString PdfFileExtension;
extern const wxString MacrosFileExtension;
extern const wxString ComponentFileExtension;
extern const wxString DrillFileExtension;
extern const wxString ReportFileExtension;
extern const wxString FootprintPlaceFileExtension;

/// Proper wxFileDialog wild card definitions.
extern const wxString ProjectFileWildcard;
extern const wxString SchematicFileWildcard;
extern const wxString BoardFileWildcard;
extern const wxString NetlistFileWildcard;
extern const wxString GerberFileWildcard;
extern const wxString PcbFileWildcard;
extern const wxString PdfFileWildcard;
extern const wxString MacrosFileWildcard;
extern const wxString AllFilesWildcard;
extern const wxString ComponentFileWildcard;
extern const wxString DrillFileWildcard;
extern const wxString ReportFileWildcard;
extern const wxString FootprintPlaceFileWildcard;
extern const wxString VrmlFileWildcard;
extern const wxString DocModulesFileName;
extern const wxString FootprintLibFileWildcard;

#endif  // INCLUDE_WILDCARDS_AND_FILES_EXT_H_
