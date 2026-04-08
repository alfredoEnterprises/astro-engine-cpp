#ifndef AE_DATA_TYPES_H
#define AE_DATA_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------
// ENUMS AND STRUCTS MUST COME FIRST
// ---------------------------------------------

typedef enum {
    AE_BODY_SUN = 0,
    AE_BODY_MOON,
    AE_BODY_MERCURY,
    AE_BODY_VENUS,
    AE_BODY_MARS,
    AE_BODY_JUPITER,
    AE_BODY_SATURN,
    AE_BODY_URANUS,
    AE_BODY_NEPTUNE,
    AE_BODY_PLUTO,
    AE_BODY_NODE,
    AE_BODY_LILITH,
    AE_BODY_CHIRON,
    AE_BODY_FORTUNE,
    AE_BODY_VERTEX,
    AE_BODY_ASC,
    AE_BODY_MC,
    AE_BODY_COUNT
} AeBodyId;

typedef struct {
    double lon;
    double lat;
    double dist;
    double speed_lon;
    int    house;
    int    retrograde;
} AeBodyState;

typedef struct {
    double cusp[13]; // 1..12
} AeHouses;

typedef enum {
    AE_ASPECT_CONJ = 0,
    AE_ASPECT_OPP,
    AE_ASPECT_TRINE,
    AE_ASPECT_SQUARE,
    AE_ASPECT_SEXTILE,
    AE_ASPECT_NONE
} AeAspectType;

typedef struct {
    AeBodyId a;
    AeBodyId b;
    AeAspectType type;
    double orb;
    int    applying;
} AeAspect;

typedef struct {
    double jd_ut;
    double lst;
    AeBodyState bodies[AE_BODY_COUNT];
    AeHouses    houses;
    AeAspect   *aspects;
    int         aspect_count;
} AeChartCore;

typedef struct {
    int year, month, day;
    int hour, minute, second;
    double timezone;
    double lat;
    double lon;
    char house_system;
} AeInput;

typedef struct {
    double longitude;   // ecliptic longitude in degrees
} AeAngleCore;

typedef struct {
    int house;          // 1..12
    double longitude;   // cusp longitude in degrees
} AeHouseCuspCore;

typedef struct {
    double jd_ut;     // Julian Day UT
    double lst_deg;   // Local Sidereal Time (degrees)
    double lat;       // latitude
    double lon;       // longitude
} AeMetadataCore;


// ---------------------------------------------
// NOW the function declarations can appear
// ---------------------------------------------

void ae_init_input(AeInput *in);
void ae_init_chart_core(AeChartCore *c);

#ifdef __cplusplus
}
#endif

#endif
