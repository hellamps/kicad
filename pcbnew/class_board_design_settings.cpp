/**
 * @file class_board_design_settings.cpp
 * BOARD_DESIGN_SETTINGS class functions.
 */

#include <fctsys.h>
#include <common.h>
#include <layers_id_colors_and_visibility.h>

#include <pcbnew.h>
#include <class_board_design_settings.h>

#include <class_track.h>


BOARD_DESIGN_SETTINGS::BOARD_DESIGN_SETTINGS() :
    m_Pad_Master( 0 )
{
    m_EnabledLayers = ALL_LAYERS;               // All layers enabled at first.
                                                // SetCopperLayerCount() will adjust this.

    SetVisibleLayers( FULL_LAYERS );

    // set all but hidden text as visible.
    m_VisibleElements = ~( 1 << MOD_TEXT_INVISIBLE );

    SetCopperLayerCount( 2 );                   // Default design is a double sided board

    // via type (VIA_BLIND_BURIED, VIA_THROUGH VIA_MICROVIA).
    m_CurrentViaType = VIA_THROUGH;

    // if true, when creating a new track starting on an existing track, use this track width
    m_UseConnectedTrackWidth = false;

    m_MicroViasAllowed = false;                 // true to allow micro vias
    m_DrawSegmentWidth = 100;                   // current graphic line width (not EDGE layer)
    m_EdgeSegmentWidth = 100;                   // current graphic line width (EDGE layer only)
    m_PcbTextWidth     = 100;                   // current Pcb (not module) Text width
    m_PcbTextSize       = wxSize( 500, 500 );   // current Pcb (not module) Text size
    m_TrackMinWidth     = 100;                  // track min value for width ((min copper size value
    m_ViasMinSize       = 350;                  // vias (not micro vias) min diameter
    m_ViasMinDrill      = 200;                  // vias (not micro vias) min drill diameter
    m_MicroViasMinSize  = 200;                  // micro vias (not vias) min diameter
    m_MicroViasMinDrill = 50;                   // micro vias (not vias) min drill diameter

    // Global mask margins:
    m_SolderMaskMargin  = 150;                  // Solder mask margin
    m_SolderPasteMargin = 0;                    // Solder paste margin absolute value
    m_SolderPasteMarginRatio = 0.0;             // Solder pask margin ratio value of pad size
                                                // The final margin is the sum of these 2 values
                                                // Usually < 0 because the mask is smaller than pad

    m_ModuleTextSize = wxSize( 500, 500 );
    m_ModuleTextWidth = 100;
    m_ModuleSegmentWidth = 100;


    // Layer thickness for 3D viewer
    m_BoardThickness = (int)(1.6 * PCB_INTERNAL_UNIT / 25.4);
}


void BOARD_DESIGN_SETTINGS::AppendConfigs( PARAM_CFG_ARRAY* aResult )
{
    m_Pad_Master.AppendConfigs( aResult );

    aResult->push_back( new PARAM_CFG_INT( wxT( "BoardThickness" ),
                                                      &m_BoardThickness,
                                                      630, 0, 0xFFFF ) );

    aResult->push_back( new PARAM_CFG_INT( wxT( "TxtPcbV" ),
                                                      &m_PcbTextSize.y,
                                                      600, TEXTS_MIN_SIZE, TEXTS_MAX_SIZE ) );

    aResult->push_back( new PARAM_CFG_INT( wxT( "TxtPcbH" ),
                                                      &m_PcbTextSize.x,
                                                      600, TEXTS_MIN_SIZE, TEXTS_MAX_SIZE ) );

    aResult->push_back( new PARAM_CFG_INT( wxT( "TxtModV" ), &m_ModuleTextSize.y,
                                                      500, TEXTS_MIN_SIZE, TEXTS_MAX_SIZE ) );
    aResult->push_back( new PARAM_CFG_INT( wxT( "TxtModH" ), &m_ModuleTextSize.x,
                                                      500, TEXTS_MIN_SIZE, TEXTS_MAX_SIZE ) );
    aResult->push_back( new PARAM_CFG_INT( wxT( "TxtModW" ), &m_ModuleTextWidth,
                                                      100, 1, TEXTS_MAX_WIDTH ) );

    aResult->push_back( new PARAM_CFG_INT( wxT( "VEgarde" ),
                                                      &m_SolderMaskMargin,
                                                      100, 0, 10000 ) );

    aResult->push_back( new PARAM_CFG_INT( wxT( "DrawLar" ),
                                                      &m_DrawSegmentWidth,
                                                      120, 0, 0xFFFF ) );

    aResult->push_back( new PARAM_CFG_INT( wxT( "EdgeLar" ),
                                                      &m_EdgeSegmentWidth,
                                                      120, 0, 0xFFFF ) );
    aResult->push_back( new PARAM_CFG_INT( wxT( "TxtLar" ),
                                                      &m_PcbTextWidth,
                                                      120, 0, 0xFFFF ) );
    aResult->push_back( new PARAM_CFG_INT( wxT( "MSegLar" ), &m_ModuleSegmentWidth,
                                                      120, 0, 0xFFFF ) );
}


// see pcbstruct.h
int BOARD_DESIGN_SETTINGS::GetVisibleLayers() const
{
    return m_VisibleLayers;
}


void BOARD_DESIGN_SETTINGS::SetVisibleAlls()
{
    SetVisibleLayers( FULL_LAYERS );
    m_VisibleElements = -1;
}


void BOARD_DESIGN_SETTINGS::SetVisibleLayers( int aMask )
{
    // Although Pcbnew uses only 29, GerbView uses all 32 layers
    m_VisibleLayers = aMask & m_EnabledLayers & FULL_LAYERS;
}


void BOARD_DESIGN_SETTINGS::SetLayerVisibility( int aLayerIndex, bool aNewState )
{
    // Altough Pcbnew uses only 29, GerbView uses all 32 layers
    if( aLayerIndex < 0 || aLayerIndex >= 32 )
        return;

    if( aNewState && IsLayerEnabled( aLayerIndex ) )
        m_VisibleLayers |= 1 << aLayerIndex;
    else
        m_VisibleLayers &= ~( 1 << aLayerIndex );
}


void BOARD_DESIGN_SETTINGS::SetElementVisibility( int aElementCategory, bool aNewState )
{
    if( aElementCategory < 0 || aElementCategory >= END_PCB_VISIBLE_LIST )
        return;

    if( aNewState )
        m_VisibleElements |= 1 << aElementCategory;
    else
        m_VisibleElements &= ~( 1 << aElementCategory );
}


void BOARD_DESIGN_SETTINGS::SetCopperLayerCount( int aNewLayerCount )
{
    // if( aNewLayerCount < 2 ) aNewLayerCount = 2;

    m_CopperLayerCount = aNewLayerCount;

    // ensure consistency with the m_EnabledLayers member
    m_EnabledLayers &= ~ALL_CU_LAYERS;
    m_EnabledLayers |= LAYER_BACK;

    if( m_CopperLayerCount > 1 )
        m_EnabledLayers |= LAYER_FRONT;

    for( int ii = 1; ii < aNewLayerCount - 1; ii++ )
        m_EnabledLayers |= 1 << ii;
}


void BOARD_DESIGN_SETTINGS::SetEnabledLayers( int aMask )
{
    // Back and front layers are always enabled.
    aMask |= LAYER_BACK | LAYER_FRONT;

    m_EnabledLayers = aMask;

    // A disabled layer cannot be visible
    m_VisibleLayers &= aMask;

    // update m_CopperLayerCount to ensure its consistency with m_EnabledLayers
    m_CopperLayerCount = 0;

    for( int ii = 0;  aMask && ii < NB_COPPER_LAYERS;  ii++, aMask >>= 1 )
    {
        if( aMask & 1 )
            m_CopperLayerCount++;
    }
}
