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

// Record one eclipse into the window (longitude from the Sun for solar,
// Moon for lunar, computed at the eclipse maximum).
static int t_record_eclipse(AeTransitWindow *out, AeEclipseKind kind,
                            double jd_max, int type_flags,
                            char *err, int err_size) {
    if (out->eclipse_count >= AE_TRANSIT_MAX_ECLIPSES) return 0; // cap silently

    AeBodyId ref = (kind == AE_ECLIPSE_SOLAR) ? AE_BODY_SUN : AE_BODY_MOON;
    AeBodyState bs;
    memset(&bs, 0, sizeof(bs));
    if (ae_compute_body(jd_max, ref, &bs, err, err_size) != 0) return -1;

    AeEclipseEvent *e = &out->eclipses[out->eclipse_count++];
    e->kind       = kind;
    e->jd_ut_max  = jd_max;
    e->lon        = bs.lon;
    e->type_flags = type_flags;
    return 0;
}

int ae_find_eclipses(double start_jd_ut, int day_count,
                     AeTransitWindow *out, char *err, int err_size) {
    if (!out) { t_set_error(err, err_size, "NULL AeTransitWindow"); return -1; }
    double end_jd = start_jd_ut + (double)day_count;

    swe_set_ephe_path("ephe");

    // Solar eclipses (global search, any type).
    double t = start_jd_ut;
    for (int guard = 0; guard < 64; guard++) {
        double tret[10]; char serr[256] = {0};
        int rc = swe_sol_eclipse_when_glob(t, SEFLG_SWIEPH, 0, tret, 0, serr);
        if (rc < 0) break;
        if (tret[0] >= end_jd) break;
        if (tret[0] >= start_jd_ut) {
            if (t_record_eclipse(out, AE_ECLIPSE_SOLAR, tret[0], rc, err, err_size) != 0)
                return -1;
        }
        t = tret[0] + 1.0; // advance past this eclipse
        if (out->eclipse_count >= AE_TRANSIT_MAX_ECLIPSES) break;
    }

    // Lunar eclipses (any type).
    t = start_jd_ut;
    for (int guard = 0; guard < 64; guard++) {
        double tret[10]; char serr[256] = {0};
        int rc = swe_lun_eclipse_when(t, SEFLG_SWIEPH, 0, tret, 0, serr);
        if (rc < 0) break;
        if (tret[0] >= end_jd) break;
        if (tret[0] >= start_jd_ut) {
            if (t_record_eclipse(out, AE_ECLIPSE_LUNAR, tret[0], rc, err, err_size) != 0)
                return -1;
        }
        t = tret[0] + 1.0;
        if (out->eclipse_count >= AE_TRANSIT_MAX_ECLIPSES) break;
    }

    return 0;
}
