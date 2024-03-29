/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
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

/**
 * @file eeschema/files-io.cpp
 */

#include <fctsys.h>
#include <class_drawpanel.h>
#include <confirm.h>
#include <gestfich.h>
#include <wxEeschemaStruct.h>

#include <general.h>
#include <protos.h>
#include <eeschema_id.h>
#include <class_library.h>
#include <libeditframe.h>
#include <sch_sheet.h>
#include <wildcards_and_files_ext.h>


bool SCH_EDIT_FRAME::SaveEEFile( SCH_SCREEN* aScreen, int aSaveType, bool aCreateBackupFile )
{
    wxString msg;
    wxFileName schematicFileName, backupFileName;
    FILE* f;

    if( aScreen == NULL )
        aScreen = GetScreen();

    /* If no name exists in the window yet - save as new. */
    if( aScreen->GetFileName().IsEmpty() )
        aSaveType = FILE_SAVE_NEW;

    switch( aSaveType )
    {
    case FILE_SAVE_AS:
        schematicFileName = aScreen->GetFileName();

        // Sheet file names are relative to the root sheet path which is the current
        // working directory.  The IsWritable funtion expects the path to be set.
        if( schematicFileName.GetPath().IsEmpty() )
            schematicFileName.Assign( wxFileName::GetCwd(), schematicFileName.GetFullName() );

        if( aCreateBackupFile )
        {
            backupFileName = schematicFileName;

            if( !IsWritable( schematicFileName ) )
                return false;

            /* Rename the old file to a '.bak' one: */
            if( schematicFileName.FileExists() )
            {
                backupFileName.SetExt( g_SchematicBackupFileExtension );
                if( backupFileName.FileExists() )
                    wxRemoveFile( backupFileName.GetFullPath() );

                if( !wxRenameFile( schematicFileName.GetFullPath(), backupFileName.GetFullPath() ) )
                {
                    msg.Printf( _( "Could not save backup of file <%s>" ),
                                GetChars( schematicFileName.GetFullPath() ) );
                    DisplayError( this, msg );
                }
            }
        }

        break;

    case FILE_SAVE_NEW:
    {
        schematicFileName = aScreen->GetFileName();

        wxFileDialog dlg( this, _( "Schematic Files" ), wxGetCwd(),
                          schematicFileName.GetFullName(), SchematicFileWildcard,
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

        if( dlg.ShowModal() == wxID_CANCEL )
            return false;

        aScreen->SetFileName( dlg.GetPath() );
        schematicFileName = dlg.GetPath();

        if( !IsWritable( schematicFileName ) )
            return false;

        break;
    }

    default:
        break;
    }

    wxLogTrace( traceAutoSave,
                wxT( "Saving file <" ) + schematicFileName.GetFullPath() + wxT( ">" ) );

    if( ( f = wxFopen( schematicFileName.GetFullPath(), wxT( "wt" ) ) ) == NULL )
    {
        msg.Printf( _( "Failed to create file <%s>" ),
                    GetChars( schematicFileName.GetFullPath() ) );
        DisplayError( this, msg );
        return false;
    }

    if( aSaveType == FILE_SAVE_NEW )
        aScreen->SetFileName( schematicFileName.GetFullPath() );

    bool success = aScreen->Save( f );

    if( !success )
    {
        DisplayError( this, _( "File write operation failed." ) );
    }
    else
    {
        // Delete auto save file on successful save.
        wxFileName autoSaveFileName = schematicFileName;
        autoSaveFileName.SetName( wxT( "$" ) + schematicFileName.GetName() );

        if( autoSaveFileName.FileExists() )
        {
            wxLogTrace( traceAutoSave,
                        wxT( "Removing auto save file <" ) + autoSaveFileName.GetFullPath() +
                        wxT( ">" ) );

            wxRemoveFile( autoSaveFileName.GetFullPath() );
        }

        aScreen->ClrSave();
        aScreen->ClrModify();
        wxString msg;
        msg.Printf( _( "File %s saved" ), GetChars( aScreen->GetFileName() ) );
        SetStatusText( msg, 0 );
    }


    fclose( f );

    return success;
}


void SCH_EDIT_FRAME::Save_File( wxCommandEvent& event )
{
    int id = event.GetId();

    switch( id )
    {
    case ID_SAVE_ONE_SHEET:     /* Update Schematic File */
        SaveEEFile( NULL, FILE_SAVE_AS );
        break;

    case ID_SAVE_ONE_SHEET_AS:     /* Save EED (new name) */
        SaveEEFile( NULL, FILE_SAVE_NEW );
        break;

    default:
        DisplayError( this, wxT( "SCH_EDIT_FRAME::Save_File Internal Error" ) );
        break;
    }
}


bool SCH_EDIT_FRAME::LoadOneEEProject( const wxString& aFileName, bool aIsNew )
{
    SCH_SCREEN* screen;
    wxString    FullFileName, msg;
    bool        LibCacheExist = false;
    SCH_SCREENS ScreenList;

    for( screen = ScreenList.GetFirst(); screen != NULL; screen = ScreenList.GetNext() )
    {
        if( screen->IsModify() )
            break;
    }

    if( screen )
    {
        if( !IsOK( this, _( "Discard changes to the current schematic?" ) ) )
            return false;
    }

    FullFileName = aFileName;

    if( ( FullFileName.IsEmpty() ) && !aIsNew )
    {
        wxFileDialog dlg( this, _( "Open Schematic" ), wxGetCwd(),
                          wxEmptyString, SchematicFileWildcard,
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST );

        if( dlg.ShowModal() == wxID_CANCEL )
            return false;

        FullFileName = dlg.GetPath();
    }

    if( g_RootSheet )
    {
        SAFE_DELETE( g_RootSheet );
    }

    CreateScreens();
    screen = GetScreen();

    wxFileName fn = FullFileName;

    if( fn.IsRelative() )
    {
        fn.MakeAbsolute();
        FullFileName = fn.GetFullPath();
    }

    wxLogDebug( wxT( "Loading schematic " ) + FullFileName );
    wxSetWorkingDirectory( fn.GetPath() );

    screen->SetFileName( FullFileName );
    g_RootSheet->SetFileName( FullFileName );
    SetStatusText( wxEmptyString );
    ClearMsgPanel();

    screen->ClrModify();

    if( aIsNew )
    {
        /* SCH_SCREEN constructor does this now
        screen->SetPageSettings( PAGE_INFO( wxT( "A4" ) ) );
        */

        screen->SetZoom( 32 );
        screen->SetGrid( ID_POPUP_GRID_LEVEL_1000 + m_LastGridSizeId  );

        TITLE_BLOCK tb;
        wxString    title;

        title += NAMELESS_PROJECT;
        title += wxT( ".sch" );
        tb.SetTitle( title );
        screen->SetTitleBlock( tb );

        GetScreen()->SetFileName( title );

        LoadProjectFile( wxEmptyString, true );
        Zoom_Automatique( false );
        SetSheetNumberAndCount();
        m_canvas->Refresh();
        return true;
    }

    // Reloading configuration.
    msg = _( "Ready\nWorking dir: \n" ) + wxGetCwd();
    PrintMsg( msg );

    LoadProjectFile( wxEmptyString, false );

    // Clear (if needed) the current active library in libedit because it could be
    // removed from memory
    LIB_EDIT_FRAME::EnsureActiveLibExists();

    // Delete old caches.
    CMP_LIBRARY::RemoveCacheLibrary();

    /* Loading the project library cache
     * until apr 2009 the lib is named <root_name>.cache.lib
     * and after (due to code change): <root_name>-cache.lib
     * so if the <name>-cache.lib is not found, the old way will be tried
     */
    fn = g_RootSheet->GetScreen()->GetFileName();

    bool use_oldcachename = false;
    wxString cachename =  fn.GetName() + wxT( "-cache" );

    fn.SetName( cachename );
    fn.SetExt( CompLibFileExtension );

    if( ! fn.FileExists() )
    {
        fn = g_RootSheet->GetScreen()->GetFileName();
        fn.SetExt( wxT( "cache.lib" ) );
        use_oldcachename = true;
    }

    if( fn.FileExists() )
    {
        wxString errMsg;

        wxLogDebug( wxT( "LoadOneEEProject() load schematic cache library file <%s>" ),
                    GetChars( fn.GetFullPath() ) );
        msg = wxT( "Load " ) + fn.GetFullPath();

        CMP_LIBRARY* LibCache = CMP_LIBRARY::LoadLibrary( fn, errMsg );

        if( LibCache )
        {
            LibCache->SetCache();
            msg += wxT( " OK" );

            if ( use_oldcachename )     // set the new name
            {
                fn.SetName( cachename );
                fn.SetExt( CompLibFileExtension );
                LibCache->SetFileName( fn );
            }

            LibCacheExist = true;
            CMP_LIBRARY::GetLibraryList().push_back( LibCache );
        }
        else
        {
            wxString prompt;

            prompt.Printf( _( "Component library <%s> failed to load.\nError: %s" ),
                           GetChars( fn.GetFullPath() ),
                           GetChars( errMsg ) );
            DisplayError( this, prompt );
            msg += _( " ->Error" );
        }

        PrintMsg( msg );
    }

    if( !wxFileExists( g_RootSheet->GetScreen()->GetFileName() ) && !LibCacheExist )
    {
        Zoom_Automatique( false );
        msg.Printf( _( "File <%s> not found." ),
                    GetChars( g_RootSheet->GetScreen()->GetFileName() ) );
        DisplayInfoMessage( this, msg );
        return false;
    }

    // load the project.
    g_RootSheet->SetScreen( NULL );
    bool diag = g_RootSheet->Load( this );
    SetScreen( m_CurrentSheet->LastScreen() );

    UpdateFileHistory( g_RootSheet->GetScreen()->GetFileName() );

    /* Redraw base screen (ROOT) if necessary. */
    GetScreen()->SetGrid( ID_POPUP_GRID_LEVEL_1000 + m_LastGridSizeId );
    Zoom_Automatique( false );
    SetSheetNumberAndCount();
    m_canvas->Refresh( true );
    return diag;
}


void SCH_EDIT_FRAME::OnSaveProject( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen;
    wxFileName  fn;
    wxFileName  tmp;
    SCH_SCREENS ScreenList;

    fn = g_RootSheet->GetFileName();
    tmp.AssignDir( fn.GetPath() );

    if( !IsWritable( tmp ) )
        return;

    for( screen = ScreenList.GetFirst(); screen != NULL; screen = ScreenList.GetNext() )
    {
        SaveEEFile( screen, FILE_SAVE_AS );
    }

    wxString cachename = fn.GetName() + wxT( "-cache" );
    fn.SetName( cachename );
    fn.SetExt( CompLibFileExtension );
    CreateArchiveLibrary( fn.GetFullPath() );
}


bool SCH_EDIT_FRAME::doAutoSave()
{
    wxFileName tmpFileName = g_RootSheet->GetFileName();
    wxFileName fn = tmpFileName;
    wxFileName  tmp;
    SCH_SCREENS screens;
    bool autoSaveOk = true;

    tmp.AssignDir( fn.GetPath() );

    if( !IsWritable( tmp ) )
        return false;

    for( SCH_SCREEN* screen = screens.GetFirst(); screen != NULL; screen = screens.GetNext() )
    {
        // Only create auto save files for the schematics that have been modified.
        if( !screen->IsSave() )
            continue;

        tmpFileName = fn = screen->GetFileName();

        // Auto save file name is the normal file name prefixed with $.
        fn.SetName( wxT( "$" ) + fn.GetName() );

        screen->SetFileName( fn.GetFullPath() );

        if( SaveEEFile( screen, FILE_SAVE_AS, NO_BACKUP_FILE ) )
        {
            screen->SetModify();
        }
        else
        {
            autoSaveOk = false;
        }

        screen->SetFileName( tmpFileName.GetFullPath() );
    }

    if( autoSaveOk )
        m_autoSaveState = false;

    return autoSaveOk;
}
