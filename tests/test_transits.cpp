#include <cstdio>
#include <cmath>
#include <cstring>
#include "swephexp.h"
#include "transits.h"

static int failures = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { std::printf("FAIL: %s\n", msg); failures++; } \
    else         { std::printf("ok:   %s\n", msg); } \
} while (0)

int main() {
    char err[256] = {0};

    // Start at 2024-04-01 12:00 UT, 10-day window.
    double start = swe_julday(2024, 4, 1, 12.0, SE_GREG_CAL);

    AeTransitWindow w;
    std::memset(&w, 0, sizeof(w));
    int rc = ae_compute_transit_window(start, 10, &w, err, sizeof(err));

    CHECK(rc == 0, "compute returns 0");
    CHECK(w.day_count == 10, "day_count is 10");
    CHECK(std::fabs(w.days[0].jd_ut - start) < 1e-9, "day 0 jd equals start");
    CHECK(std::fabs(w.days[9].jd_ut - (start + 9.0)) < 1e-9, "day 9 jd is start+9");

    // Sun (index 0): always direct, ~ +1 deg/day.
    AeTransitBodyState sun0 = w.days[0].bodies[0];
    CHECK(sun0.retrograde == 0, "Sun never retrograde");
    CHECK(sun0.speed_lon > 0.9 && sun0.speed_lon < 1.1, "Sun speed ~ +1 deg/day");

    // All longitudes normalized [0,360).
    int lon_ok = 1;
    for (int d = 0; d < w.day_count; d++)
        for (int b = 0; b < AE_TRANSIT_BODY_COUNT; b++) {
            double L = w.days[d].bodies[b].lon;
            if (!(L >= 0.0 && L < 360.0)) lon_ok = 0;
        }
    CHECK(lon_ok, "all longitudes in [0,360)");

    // Error path: day_count out of range is rejected.
    AeTransitWindow bad;
    std::memset(&bad, 0, sizeof(bad));
    int rc_bad = ae_compute_transit_window(start, 0, &bad, err, sizeof(err));
    CHECK(rc_bad == -1, "day_count 0 rejected");

    // Eclipse search: 100-day window from 2024-04-01 contains the
    // 2024-04-08 total solar eclipse at ecliptic longitude ~19 deg.
    AeTransitWindow we;
    std::memset(&we, 0, sizeof(we));
    double estart = swe_julday(2024, 4, 1, 0.0, SE_GREG_CAL);
    int erc = ae_compute_transit_window(estart, 100, &we, err, sizeof(err));
    CHECK(erc == 0, "eclipse-window compute returns 0");
    int ferc = ae_find_eclipses(estart, 100, &we, err, sizeof(err));
    CHECK(ferc == 0, "find_eclipses returns 0");
    CHECK(we.eclipse_count >= 1, "at least one eclipse found in 100 days");

    int found_solar_lon19 = 0;
    for (int i = 0; i < we.eclipse_count; i++) {
        AeEclipseEvent *e = &we.eclipses[i];
        int in_window = (e->jd_ut_max >= estart) && (e->jd_ut_max < estart + 100.0);
        int lon_ok2   = (e->lon >= 0.0 && e->lon < 360.0);
        if (!in_window || !lon_ok2) { std::printf("FAIL: eclipse bounds\n"); failures++; }
        if (e->kind == AE_ECLIPSE_SOLAR && e->lon > 15.0 && e->lon < 25.0)
            found_solar_lon19 = 1;
    }
    CHECK(found_solar_lon19, "solar eclipse at ecliptic longitude ~19 deg present");

    if (failures) { std::printf("\n%d CHECK(S) FAILED\n", failures); return 1; }
    std::printf("\nALL CHECKS PASSED\n");
    return 0;
}
