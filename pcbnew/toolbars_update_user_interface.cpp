/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009 Jean-Pierre Charras, jaen-pierre.charras@gipsa-lab.inpg.com
 * Copyright (C) 2007-2011 Wayne Stambaugh <stambaughw@verizon.net>
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
 * @file toolbars_update_user_interface.cpp
 * @brief Function to update toolbars UI after changing parameters.
 */

#include <fctsys.h>
#include <appl_wxstruct.h>
#include <class_drawpanel.h>
#include <wxPcbStruct.h>
#include <3d_viewer.h>
#include <dialog_helpers.h>
#include <pcbcommon.h>

#include <class_board.h>

#include <pcbnew.h>
#include <pcbnew_id.h>
#include <drc_stuff.h>


void PCB_EDIT_FRAME::OnUpdateLayerPair( wxUpdateUIEvent& aEvent )
{
    PrepareLayerIndicator();
}


void PCB_EDIT_FRAME::OnUpdateSelectTrackWidth( wxUpdateUIEvent& aEvent )
{
    if( aEvent.GetId() == ID_AUX_TOOLBAR_PCB_TRACK_WIDTH )
    {
        if( m_SelTrackWidthBox->GetSelection() != (int) GetBoard()->m_TrackWidthSelector )
            m_SelTrackWidthBox->SetSelection( GetBoard()->m_TrackWidthSelector );
    }
    else
    {
        bool check = ( ( ( ID_POPUP_PCB_SELECT_WIDTH1 +
                           (int) GetBoard()->m_TrackWidthSelector ) == aEvent.GetId() ) &&
                       !GetDesignSettings().m_UseConnectedTrackWidth );
        aEvent.Check( check );
    }
}


void PCB_EDIT_FRAME::OnUpdateSelectAutoTrackWidth( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( GetDesignSettings().m_UseConnectedTrackWidth );
}


void PCB_EDIT_FRAME::OnUpdateSelectViaSize( wxUpdateUIEvent& aEvent )
{
    wxString msg;

    if( aEvent.GetId() == ID_AUX_TOOLBAR_PCB_VIA_SIZE )
    {
        if( m_SelViaSizeBox->GetSelection() != (int) GetBoard()->m_ViaSizeSelector )
            m_SelViaSizeBox->SetSelection( GetBoard()->m_ViaSizeSelector );
    }
    else
    {
        bool check = ( ( ( ID_POPUP_PCB_SELECT_VIASIZE1 +
                           (int) GetBoard()->m_ViaSizeSelector ) == aEvent.GetId() ) &&
                       !GetDesignSettings().m_UseConnectedTrackWidth );

        aEvent.Check( check );
    }
}


void PCB_EDIT_FRAME::OnUpdateLayerSelectBox( wxUpdateUIEvent& aEvent )
{
    m_SelLayerBox->SetLayerSelection( getActiveLayer() );
}


void PCB_EDIT_FRAME::OnUpdateZoneDisplayStyle( wxUpdateUIEvent& aEvent )
{
    int selected = aEvent.GetId() - ID_TB_OPTIONS_SHOW_ZONES;

    if( aEvent.IsChecked() && ( DisplayOpt.DisplayZonesMode == selected ) )
        return;

    aEvent.Check( DisplayOpt.DisplayZonesMode == selected );
}


void PCB_EDIT_FRAME::OnUpdateDrcEnable( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( !Drc_On );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_DRC_OFF,
                                        Drc_On ?
                                        _( "Disable design rule checking" ) :
                                        _( "Enable design rule checking" ) );
}

void PCB_EDIT_FRAME::OnUpdateShowBoardRatsnest( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( GetBoard()->IsElementVisible( RATSNEST_VISIBLE ) );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_SHOW_RATSNEST,
                                        GetBoard()->IsElementVisible( RATSNEST_VISIBLE ) ?
                                        _( "Hide board ratsnest" ) :
                                        _( "Show board ratsnest" ) );
}


void PCB_EDIT_FRAME::OnUpdateShowModuleRatsnest( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( g_Show_Module_Ratsnest );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_SHOW_MODULE_RATSNEST,
                                        g_Show_Module_Ratsnest ?
                                        _( "Hide module ratsnest" ) :
                                        _( "Show module ratsnest" ) );
}


void PCB_EDIT_FRAME::OnUpdateAutoDeleteTrack( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( g_AutoDeleteOldTrack );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_AUTO_DEL_TRACK,
                                        g_AutoDeleteOldTrack ?
                                        _( "Disable auto delete old track" ) :
                                        _( "Enable auto delete old track" ) );
}


void PCB_EDIT_FRAME::OnUpdateViaDrawMode( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( !m_DisplayViaFill );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_SHOW_VIAS_SKETCH,
                                        m_DisplayViaFill ?
                                        _( "Show vias in outline mode" ) :
                                        _( "Show vias in fill mode" ) );
}


void PCB_EDIT_FRAME::OnUpdateTraceDrawMode( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( !m_DisplayPcbTrackFill );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_SHOW_TRACKS_SKETCH,
                                        m_DisplayPcbTrackFill ?
                                        _( "Show tracks in outline mode" ) :
                                        _( "Show tracks in fill mode" ) );
}


void PCB_EDIT_FRAME::OnUpdateHighContrastDisplayMode( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( DisplayOpt.ContrastModeDisplay );
    m_optionsToolBar->SetToolShortHelp( ID_TB_OPTIONS_SHOW_HIGH_CONTRAST_MODE,
                                        DisplayOpt.ContrastModeDisplay ?
                                        _( "Normal contrast display mode" ) :
                                        _( "High contrast display mode" ) );
}


void PCB_EDIT_FRAME::OnUpdateShowLayerManager( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( m_auimgr.GetPane( wxT( "m_LayersManagerToolBar" ) ).IsShown() );
}

void PCB_EDIT_FRAME::OnUpdateShowMicrowaveToolbar( wxUpdateUIEvent& aEvent )
{
    aEvent.Check( m_auimgr.GetPane( wxT( "m_microWaveToolBar" ) ).IsShown() );
}


void PCB_EDIT_FRAME::OnUpdateSave( wxUpdateUIEvent& aEvent )
{
    aEvent.Enable( GetScreen()->IsModify() );
}


void PCB_EDIT_FRAME::OnUpdateVerticalToolbar( wxUpdateUIEvent& aEvent )
{
    if( aEvent.GetEventObject() == m_drawToolBar )
        aEvent.Check( GetToolId() == aEvent.GetId() );
}


void PCB_EDIT_FRAME::OnUpdateAutoPlaceTracksMode( wxUpdateUIEvent& aEvent )
{
    // Automatic placement of modules and tracks is a mutually exclusive operation so
    // clear the other tool if one of the two is selected.
    aEvent.Check( m_autoPlaceModeId == ID_TOOLBARH_PCB_MODE_TRACKS );
}


void PCB_EDIT_FRAME::OnUpdateAutoPlaceModulesMode( wxUpdateUIEvent& aEvent )
{
    // Automatic placement of modules and tracks is a mutually exclusive operation so
    // clear the other tool if one of the two is selected.
    aEvent.Check( m_autoPlaceModeId == ID_TOOLBARH_PCB_MODE_MODULE );
}
