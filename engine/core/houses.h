#ifndef AE_HOUSES_H
#define AE_HOUSES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "data_types.h"

typedef enum {
    AE_HOUSE_PLACIDUS = 'P'
} AeHouseSystem;

typedef struct {
    AeAngleCore asc;
    AeAngleCore mc;
    AeAngleCore dsc;
    AeAngleCore ic;
    AeAngleCore vertex;
    AeAngleCore armc;
    double obliquity;
} AeHouseAnglesCore;

typedef struct {
    AeHouseCuspCore cusps[12];
} AeHouseCuspsCore;

int ae_house_of(double lon, const AeHouseCuspsCore *cusps);

int ae_compute_houses(
    const AeMetadataCore *meta,
    AeHouseSystem system,
    AeHouseAnglesCore *angles_out,
    AeHouseCuspsCore *cusps_out,
    char *err_msg,
    int err_msg_size
);

#ifdef __cplusplus
}
#endif

#endif
