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

    if (failures) { std::printf("\n%d CHECK(S) FAILED\n", failures); return 1; }
    std::printf("\nALL CHECKS PASSED\n");
    return 0;
}
