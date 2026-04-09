#include "data_types.h" 
#include "houses.h"
#include "swephexp.h"
#include <string.h>


static void ae_set_error(char *err_msg, int err_msg_size, const char *msg) {
    if (err_msg && err_msg_size > 0) {
        strncpy(err_msg, msg, err_msg_size - 1);
        err_msg[err_msg_size - 1] = '\0';
    }
}

int ae_house_of(double lon, const AeHouseCuspsCore *cusps) {
    // Normalize longitude
    while (lon < 0) lon += 360.0;
    while (lon >= 360.0) lon -= 360.0;

    // Loop through cusps 1..12
    for (int i = 1; i <= 12; i++) {
        double c1 = cusps->cusps[i - 1].longitude;
        double c2 = cusps->cusps[i % 12].longitude; // cusp 13 wraps to cusp 1

        // Normalize
        if (c2 < c1)
            c2 += 360.0;

        double L = lon;
        if (L < c1)
            L += 360.0;

        if (L >= c1 && L < c2)
            return i;
    }

    return 12; // fallback (should never happen)
}


static double ae_norm360(double x) {
    while (x < 0.0)   x += 360.0;
    while (x >= 360.) x -= 360.0;
    return x;
}

int ae_compute_houses(
    const AeMetadataCore *meta,
    AeHouseSystem system,
    AeHouseAnglesCore *angles_out,
    AeHouseCuspsCore *cusps_out,
    char *err_msg,
    int err_msg_size
) {
    if (!meta || !angles_out || !cusps_out) {
        ae_set_error(err_msg, err_msg_size, "NULL pointer in ae_compute_houses");
        return -1;
    }

    double cusps[13];   // 1..12 used
    double ascmc[10];   // ASC, MC, ARMC, Vertex, etc.

    int rc = swe_houses_ex(
        meta->jd_ut,
        SEFLG_SWIEPH,          // same flag family as bodies
        meta->lat,
        meta->lon,
        (char)system,          // 'P' for Placidus
        cusps,
        ascmc
    );

    if (rc == ERR) {
        ae_set_error(err_msg, err_msg_size, "swe_houses_ex returned error");
        return -2;
    }

    // Angles
    angles_out->asc.longitude     = ae_norm360(ascmc[SE_ASC]);
    angles_out->mc.longitude      = ae_norm360(ascmc[SE_MC]);
    angles_out->dsc.longitude     = ae_norm360(ascmc[SE_ASC] + 180.0);
    angles_out->ic.longitude      = ae_norm360(ascmc[SE_MC] + 180.0);
    angles_out->vertex.longitude  = ae_norm360(ascmc[SE_VERTEX]);
    angles_out->armc.longitude    = ae_norm360(ascmc[SE_ARMC]);
    

    // Cusps 1..12
    for (int i = 1; i <= 12; ++i) {
        cusps_out->cusps[i - 1].house = i;
        cusps_out->cusps[i - 1].longitude = ae_norm360(cusps[i]);
    }

    return 0;
}
