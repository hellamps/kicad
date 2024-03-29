/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2004-2010 Jean-Pierre Charras, jean-pierre.charras@gpisa-lab.inpg.fr
 * Copyright (C) 2010 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2010 KiCad Developers, see change_log.txt for contributors.
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


/************************************************************/
/* class_pcb_layer_widget.h : header for the layers manager */
/************************************************************/

#ifndef CLASS_PCB_LAYER_WIDGET_H_
#define CLASS_PCB_LAYER_WIDGET_H_

/**
 * Class PCB_LAYER_WIDGET
 * is here to implement the abtract functions of LAYER_WIDGET so they
 * may be tied into the PCB_EDIT_FRAME's data and so we can add a popup
 * menu which is specific to PCBNEW's needs.
 */
class PCB_LAYER_WIDGET : public LAYER_WIDGET
{
public:

    /**
     * Constructor
     * @param aParent is the parent window
     * @param aFocusOwner is the window that should be sent the focus after
     * @param aPointSize is the font point size to use within the widget.  This
     *  effectively sets the overal size of the widget via the row height and bitmap
     *  button sizes.
     */
    PCB_LAYER_WIDGET( PCB_EDIT_FRAME* aParent, wxWindow* aFocusOwner, int aPointSize = 10 );

    void ReFill();

    /**
     * Function ReFillRender
     * rebuilds Render for instance after the config is read
     */
    void ReFillRender();

    /**
     * Function SyncRenderStates
     * updates the checkboxes (checked or not) to be consistent with the current state
     * of the visibility of the visible rendering elements.
     */
    void SyncRenderStates();

    /**
     * Function SyncLayerVisibilities
     * updates each "Layer" checkbox in this layer widget according
     * to each layer's current visibility determined by IsLayerVisible(), and is
     * helpful immediately after loading a BOARD which may have state information in it.
     */
    void SyncLayerVisibilities();

    /**
     * Function SetLayersManagerTabsText
     * Update the layer manager tabs labels
     * Useful when changing Language or to set labels to a non default value
     */
    void SetLayersManagerTabsText();

    //-----<implement LAYER_WIDGET abstract callback functions>-----------
    void OnLayerColorChange( int aLayer, int aColor );
    bool OnLayerSelect( int aLayer );
    void OnLayerVisible( int aLayer, bool isVisible, bool isFinal );
    void OnRenderColorChange( int aId, int aColor );
    void OnRenderEnable( int aId, bool isEnabled );
    //-----</implement LAYER_WIDGET abstract callback functions>----------


protected:

    static const LAYER_WIDGET::ROW  s_render_rows[];

    PCB_EDIT_FRAME*    myframe;

    // popup menu ids.
#define ID_SHOW_ALL_COPPERS             wxID_HIGHEST
#define ID_SHOW_NO_COPPERS              (wxID_HIGHEST+1)
#define ID_SHOW_NO_COPPERS_BUT_ACTIVE   (wxID_HIGHEST+2)

    /**
     * Function OnRightDownLayers
     * puts up a popup menu for the layer panel.
     */
    void onRightDownLayers( wxMouseEvent& event );

    void onPopupSelection( wxCommandEvent& event );

    /// this is for the popup menu, the right click handler has to be installed
    /// on every child control within the layer panel.
    void installRightLayerClickHandler();
};

#endif  // CLASS_PCB_LAYER_WIDGET_H_
