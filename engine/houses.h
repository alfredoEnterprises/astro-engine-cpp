#ifndef AE_HOUSES_H
#define AE_HOUSES_H

#include "data_types.h"

// House system enum (extend later if needed)
typedef enum {
    AE_HOUSE_PLACIDUS = 'P'   // Swiss uses char for house system
} AeHouseSystem;

// Core angles returned by Swiss Ephemeris
typedef struct {
    AeAngleCore asc;      // Ascendant
    AeAngleCore mc;       // Midheaven
    AeAngleCore dsc;      // Descendant
    AeAngleCore ic;       // Imum Coeli
    AeAngleCore vertex;   // Vertex (from swe_houses_ex)
    AeAngleCore armc;     // Right Ascension of MC
    double obliquity;     // True obliquity of the ecliptic
} AeHouseAnglesCore;

// 12 house cusps
typedef struct {
    AeHouseCuspCore cusps[12];
} AeHouseCuspsCore;

int ae_house_of(double lon, const AeHouseCuspsCore *cusps);

// Main compute function
int ae_compute_houses(
    const AeMetadataCore *meta,
    AeHouseSystem system,
    AeHouseAnglesCore *angles_out,
    AeHouseCuspsCore *cusps_out,
    char *err_msg,
    int err_msg_size
);

#endif
