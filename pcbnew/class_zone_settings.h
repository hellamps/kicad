/**
 * @file class_zone_settings.h
 * @brief Class ZONE_SETTINGS used to handle zones parameters in dialogs.
 */

#ifndef ZONE_SETTINGS_H_
#define ZONE_SETTINGS_H_

#include "zones.h"


class ZONE_CONTAINER;


#define MAX_ZONE_CORNER_RADIUS 4000


/**
 * Class ZONE_SETTINGS
 * handles zones parameters.
 */
class ZONE_SETTINGS
{
public:
    enum {
        SMOOTHING_NONE,
        SMOOTHING_CHAMFER,
        SMOOTHING_FILLET,
        SMOOTHING_LAST
    };

    /// Mode for filling zone : 1 use segments, 0 use polygons
    int  m_FillMode;

    int  m_ZonePriority;                ///< Priority (0 ... N) of the zone

    int  m_ZoneClearance;               ///< Clearance value
    int  m_ZoneMinThickness;            ///< Min thickness value in filled areas
    int  m_NetcodeSelection;            ///< Net code selection for the current zone
    int  m_CurrentZone_Layer;           ///< Layer used to create the current zone

    /// Option to show the zone area (outlines only, short hatches or full hatches
    int  m_Zone_HatchingStyle;

    /// Option to select number of segments to approximate a circle 16 or 32 segments.
    int  m_ArcToSegmentsCount;

    long m_ThermalReliefGap;            ///< thickness of the gap in thermal reliefs
    long m_ThermalReliefCopperBridge;   ///< thickness of the copper bridge in thermal reliefs

    bool m_Zone_45_Only;

private:
    int  cornerSmoothingType;           ///< Corner smoothing type
    unsigned int  cornerRadius;         ///< Corner chamfer distance / fillet radius
    ZoneConnection m_PadConnection;

public:
    ZONE_SETTINGS();

    /**
     * operator << ( const ZONE_CONTAINER& )
     * was Function ImportSetting
     * copies settings from a given zone into this object.
     * @param aSource: the given zone
     */
    ZONE_SETTINGS& operator << ( const ZONE_CONTAINER& aSource );

    /**
     * Function ExportSetting
     * copy settings to a given zone
     * @param aTarget: the given zone
     * @param aFullExport: if false: some parameters are NOT exported
     *   because they must not be  exported when export settings from a zone to others zones
     *   Currently:
     *      m_NetcodeSelection
     */
    void ExportSetting( ZONE_CONTAINER& aTarget, bool aFullExport = true ) const;

    void SetCornerSmoothingType( int aType) { cornerSmoothingType = aType; }

    int GetCornerSmoothingType() const { return cornerSmoothingType; }

    void SetCornerRadius( int aRadius )
    {
        if( aRadius > MAX_ZONE_CORNER_RADIUS )
            cornerRadius = MAX_ZONE_CORNER_RADIUS;
        else if( aRadius < 0 )
            cornerRadius = 0;
        else
            cornerRadius = aRadius;
    };

    unsigned int GetCornerRadius() const { return cornerRadius; }

    ZoneConnection GetPadConnection() const { return m_PadConnection; }
    void SetPadConnection( ZoneConnection aPadConnection ) { m_PadConnection = aPadConnection; }

};


#endif  // ZONE_SETTINGS_H_
