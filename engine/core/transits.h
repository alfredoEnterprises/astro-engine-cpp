#ifndef AE_TRANSITS_H
#define AE_TRANSITS_H

#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AE_TRANSIT_MAX_DAYS      100
#define AE_TRANSIT_MAX_ECLIPSES  16
// Transiting bodies = enum indices AE_BODY_SUN(0) .. AE_BODY_CHIRON(12).
#define AE_TRANSIT_BODY_COUNT    13

// One transiting body's state on a given day.
typedef struct {
    double lon;        // ecliptic longitude, normalized [0,360)
    double speed_lon;  // longitudinal speed, deg/day
    int    retrograde; // 1 if speed_lon < 0, else 0
} AeTransitBodyState;

// All transiting bodies for a single sampled day.
typedef struct {
    double jd_ut;                                    // JD(UT) of this day's sample
    AeTransitBodyState bodies[AE_TRANSIT_BODY_COUNT];
} AeTransitDay;

typedef enum {
    AE_ECLIPSE_SOLAR = 0,
    AE_ECLIPSE_LUNAR = 1
} AeEclipseKind;

typedef struct {
    AeEclipseKind kind;
    double jd_ut_max;   // JD(UT) of eclipse maximum
    double lon;         // ecliptic longitude of the eclipse point, [0,360)
    int    type_flags;  // Swiss Ephemeris eclipse-type flags (SE_ECL_TOTAL, etc.)
} AeEclipseEvent;

typedef struct {
    double start_jd_ut;    // JD(UT) of day 0's sample
    int    day_count;      // number of sampled days
    AeTransitDay   days[AE_TRANSIT_MAX_DAYS];
    int    eclipse_count;
    AeEclipseEvent eclipses[AE_TRANSIT_MAX_ECLIPSES];
} AeTransitWindow;

// Parsed transit-window request (UT datetime + day count).
typedef struct {
    int year, month, day;
    int hour, minute, second;
    int days;
} AeTransitRequest;

// Convert a request's UT datetime to JD(UT) using swe_julday + Gregorian calendar.
double ae_transit_start_jd(const AeTransitRequest *req);

// Compute positions for `day_count` days starting at start_jd_ut (one sample/day).
// Returns 0 on success, -1 on error (err filled).
int ae_compute_transit_window(double start_jd_ut, int day_count,
                              AeTransitWindow *out, char *err, int err_size);

// Append solar+lunar eclipses with max in [start_jd_ut, start_jd_ut+day_count)
// into out->eclipses / out->eclipse_count. Returns 0 on success, -1 on error.
int ae_find_eclipses(double start_jd_ut, int day_count,
                     AeTransitWindow *out, char *err, int err_size);

#ifdef __cplusplus
}
#endif

#endif
