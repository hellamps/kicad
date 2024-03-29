/******************************************************/
/* class ZONE_SETTINGS used to handle zones parameters */
/******************************************************/

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

/* For compilers that support precompilation:
 */
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <fctsys.h>

#include <common.h>
#include <pcbnew.h>
#include <zones.h>

#include <class_zone.h>

ZONE_SETTINGS::ZONE_SETTINGS()
{
    m_ZonePriority = 0;
    m_FillMode = 0;                                            // Mode for filling zone : 1 use segments, 0 use polygons
    m_ZoneClearance      = 200;                                // Clearance value
    m_ZoneMinThickness   = 100;                                // Min thickness value in filled areas
    m_NetcodeSelection   = 0;                                  // Net code selection for the current zone
    m_CurrentZone_Layer  = 0;                                  // Layer used to create the current zone
    m_Zone_HatchingStyle = CPolyLine::DIAGONAL_EDGE;           // Option to show the zone area (outlines only, short hatches or full hatches

    m_ArcToSegmentsCount = ARC_APPROX_SEGMENTS_COUNT_LOW_DEF;  // Option to select number of segments to approximate a circle
                                                               // ARC_APPROX_SEGMENTS_COUNT_LOW_DEF
                                                               // or ARC_APPROX_SEGMENTS_COUNT_HIGHT_DEF segments

    m_ThermalReliefGap = 200;                                  // tickness of the gap in thermal reliefs
    m_ThermalReliefCopperBridge = 200;                         // tickness of the copper bridge in thermal reliefs

    m_PadConnection = THERMAL_PAD;                             // How pads are covered by copper in zone

    m_Zone_45_Only = false;

    cornerSmoothingType = SMOOTHING_NONE;
    cornerRadius = 0;
}


ZONE_SETTINGS& ZONE_SETTINGS::operator << ( const ZONE_CONTAINER& aSource )
{
    m_ZonePriority = aSource.GetPriority();
    m_FillMode     = aSource.m_FillMode;
    m_ZoneClearance      = aSource.m_ZoneClearance;
    m_ZoneMinThickness   = aSource.m_ZoneMinThickness;
    m_NetcodeSelection   = aSource.GetNet();
    m_CurrentZone_Layer  = aSource.GetLayer();
    m_Zone_HatchingStyle = aSource.GetHatchStyle();
    m_ArcToSegmentsCount = aSource.m_ArcToSegmentsCount;
    m_ThermalReliefGap = aSource.m_ThermalReliefGap;
    m_ThermalReliefCopperBridge = aSource.m_ThermalReliefCopperBridge;
    m_PadConnection = aSource.GetPadConnection();
    cornerSmoothingType = aSource.GetCornerSmoothingType();
    cornerRadius = aSource.GetCornerRadius();

    return *this;
}


void ZONE_SETTINGS::ExportSetting( ZONE_CONTAINER& aTarget, bool aFullExport ) const
{
    aTarget.m_FillMode = m_FillMode;
    aTarget.m_ZoneClearance    = m_ZoneClearance;
    aTarget.m_ZoneMinThickness = m_ZoneMinThickness;
    aTarget.m_Poly->SetHatch( m_Zone_HatchingStyle );
    aTarget.m_ArcToSegmentsCount = m_ArcToSegmentsCount;
    aTarget.m_ThermalReliefGap = m_ThermalReliefGap;
    aTarget.m_ThermalReliefCopperBridge = m_ThermalReliefCopperBridge;
    aTarget.SetPadConnection( m_PadConnection );
    aTarget.SetCornerSmoothingType( cornerSmoothingType );
    aTarget.SetCornerRadius( cornerRadius );

    if( aFullExport )
    {
        aTarget.SetPriority( m_ZonePriority );
        aTarget.SetNet( m_NetcodeSelection );
        aTarget.SetLayer( m_CurrentZone_Layer );
    }
}
