#include "bodies.h"
#include "swephexp.h"
#include "houses.h"
#include <string.h>

static void ae_set_error(char *err_msg, int err_msg_size, const char *msg) {
    if (err_msg && err_msg_size > 0) {
        strncpy(err_msg, msg, err_msg_size - 1);
        err_msg[err_msg_size - 1] = '\0';
    }
}

static double ae_norm_deg(double x) {
    while (x < 0.0)   x += 360.0;
    while (x >= 360.) x -= 360.0;
    return x;
}

static int ae_body_to_swe_id(AeBodyId id, int *swe_id) {
    switch (id) {
        case AE_BODY_SUN:     *swe_id = SE_SUN;        return 0;
        case AE_BODY_MOON:    *swe_id = SE_MOON;       return 0;
        case AE_BODY_MERCURY: *swe_id = SE_MERCURY;    return 0;
        case AE_BODY_VENUS:   *swe_id = SE_VENUS;      return 0;
        case AE_BODY_MARS:    *swe_id = SE_MARS;       return 0;
        case AE_BODY_JUPITER: *swe_id = SE_JUPITER;    return 0;
        case AE_BODY_SATURN:  *swe_id = SE_SATURN;     return 0;
        case AE_BODY_URANUS:  *swe_id = SE_URANUS;     return 0;
        case AE_BODY_NEPTUNE: *swe_id = SE_NEPTUNE;    return 0;
        case AE_BODY_PLUTO:   *swe_id = SE_PLUTO;      return 0;
        case AE_BODY_NODE:    *swe_id = SE_MEAN_NODE;  return 0;
        case AE_BODY_LILITH:  *swe_id = SE_MEAN_APOG;  return 0;
        case AE_BODY_CHIRON:  *swe_id = SE_CHIRON;     return 0;
        default:
            return -1; // Fortune, Vertex, angles handled elsewhere later
    }
}

int ae_compute_body(
    double jd_ut,
    AeBodyId id,
    AeBodyState *out,
    char *err_msg,
    int err_msg_size
) {
    if (!out) {
        ae_set_error(err_msg, err_msg_size, "NULL AeBodyState");
        return -1;
    }

    int swe_id = 0;
    if (ae_body_to_swe_id(id, &swe_id) != 0) {
        ae_set_error(err_msg, err_msg_size, "Unsupported body id in ae_compute_body");
        return -1;
    }

    int iflag = SEFLG_SWIEPH | SEFLG_SPEED;  // tropical, geocentric, SWIEPH
    double xx[6];
    char serr[256] = {0};

    // Use UT directly (this is what your old engine effectively did)
    int ret = swe_calc_ut(jd_ut, swe_id, iflag, xx, serr);
    if (ret < 0) {
        ae_set_error(err_msg, err_msg_size, serr[0] ? serr : "swe_calc_ut error");
        return -1;
    }

    out->lon        = ae_norm_deg(xx[0]);
    out->lat        = xx[1];
    out->dist       = xx[2];
    out->speed_lon  = xx[3];
    out->retrograde = (xx[3] < 0.0) ? 1 : 0;
    out->house      = 0;

    return 0;
}


int ae_compute_core_bodies(
    double jd_ut,
    AeChartCore *chart,
    char *err_msg,
    int err_msg_size
) {
    if (!chart) {
        ae_set_error(err_msg, err_msg_size, "NULL AeChartCore");
        return -1;
    }

    for (int i = 0; i < AE_BODY_COUNT; ++i) {
        AeBodyId id = (AeBodyId)i;

        // Skip angles and derived points for now; they’ll be set by houses/points modules
        if (id == AE_BODY_ASC || id == AE_BODY_MC ||
            id == AE_BODY_FORTUNE || id == AE_BODY_VERTEX) {
            continue;
        }

        int rc = ae_compute_body(jd_ut, id, &chart->bodies[id], err_msg, err_msg_size);
        if (rc != 0) {
            return rc;
        }
    }

    return 0;
}

