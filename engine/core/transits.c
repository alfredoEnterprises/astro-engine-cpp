#include "transits.h"
#include "bodies.h"
#include "swephexp.h"
#include <string.h>

static void t_set_error(char *err, int err_size, const char *msg) {
    if (err && err_size > 0) {
        strncpy(err, msg, (size_t)err_size - 1);
        err[err_size - 1] = '\0';
    }
}

double ae_transit_start_jd(const AeTransitRequest *req) {
    double ut_hour = (double)req->hour
                   + (double)req->minute / 60.0
                   + (double)req->second / 3600.0;
    return swe_julday(req->year, req->month, req->day, ut_hour, SE_GREG_CAL);
}

int ae_compute_transit_window(double start_jd_ut, int day_count,
                              AeTransitWindow *out, char *err, int err_size) {
    if (!out) { t_set_error(err, err_size, "NULL AeTransitWindow"); return -1; }
    if (day_count < 1 || day_count > AE_TRANSIT_MAX_DAYS) {
        t_set_error(err, err_size, "day_count out of range (1..100)");
        return -1;
    }

    // Match ae_compute_core_bodies: ensure the ephemeris path is set.
    swe_set_ephe_path("ephe");

    out->start_jd_ut   = start_jd_ut;
    out->day_count     = day_count;
    out->eclipse_count = 0;

    for (int d = 0; d < day_count; d++) {
        double jd = start_jd_ut + (double)d;
        out->days[d].jd_ut = jd;

        for (int bi = 0; bi < AE_TRANSIT_BODY_COUNT; bi++) {
            AeBodyState bs;
            memset(&bs, 0, sizeof(bs));
            if (ae_compute_body(jd, (AeBodyId)bi, &bs, err, err_size) != 0)
                return -1;
            out->days[d].bodies[bi].lon        = bs.lon;
            out->days[d].bodies[bi].speed_lon  = bs.speed_lon;
            out->days[d].bodies[bi].retrograde = bs.retrograde;
        }
    }
    return 0;
}
