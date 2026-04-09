#include "aspects.h"
#include "aspect_rules.h"
#include "data_types.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define AE_MAX_ASPECTS 512

// ------------------------------------------------------------
// Normalize angle to [-180, 180]
// ------------------------------------------------------------
static double norm180(double x)
{
    while (x > 180.0)  x -= 360.0;
    while (x < -180.0) x += 360.0;
    return x;
}

// ------------------------------------------------------------
// FIND CLOSEST ASPECT TYPE
// ------------------------------------------------------------
AeAspectType ae_find_closest_aspect(double sep, double *delta_out)
{
    double sep_in = sep;
    sep = norm180(sep);

    AeAspectType best_type  = AE_ASPECT_NONE;
    double       best_abs   = 999.0;

    for (int t = AE_ASPECT_CONJUNCTION; t <= AE_ASPECT_SEXTILE; ++t) {
        double exact = AE_ASPECT_ANGLE[t];
        double sabs  = fabs(sep);
        double ad    = fabs(sabs - exact);

        if (ad < best_abs) {
            best_abs  = ad;
            best_type = (AeAspectType)t;
        }
    }

    double signed_delta = (sep >= 0.0 ? 1.0 : -1.0) * best_abs;

    if (delta_out)
        *delta_out = signed_delta;

    return best_type;
}





// ------------------------------------------------------------
// APPLYING / SEPARATING
// ------------------------------------------------------------
bool ae_is_applying(
    double lon_a, double speed_a,
    double lon_b, double speed_b,
    AeAspectType type
) {
    double exact = AE_ASPECT_ANGLE[type];
    double sep = norm180(lon_b - lon_a);
    double delta = norm180(sep - exact);

    // If delta > 0, they haven't reached exact yet.
    // If (speed_b - speed_a) < 0, b is catching up to a.
    if (delta > 0.0 && (speed_b - speed_a) < 0.0)
        return true;

    // If delta < 0, they passed exact.
    // If (speed_b - speed_a) > 0, they are moving back toward exact.
    if (delta < 0.0 && (speed_b - speed_a) > 0.0)
        return true;

    return false;
}

// ------------------------------------------------------------
// MAIN ASPECT ENGINE
// ------------------------------------------------------------
int ae_compute_aspects(AeChartCore *chart)
{
    if (!chart)
        return -1;

    chart->aspect_count = 0;

    for (int a = 0; a < AE_BODY_COUNT; a++) {
        for (int b = a + 1; b < AE_BODY_COUNT; b++) {

            // Inclusion rules (angle-angle forbidden, etc.)
            if (!ae_aspect_allowed(a, b))
                continue;

            double lon_a = chart->bodies[a].lon;
            double lon_b = chart->bodies[b].lon;
            double speed_a = chart->bodies[a].speed_lon;
            double speed_b = chart->bodies[b].speed_lon;

            // ------------------------------------------------------------
            // CRITICAL FIX: normalize separation HERE, not elsewhere
            // ------------------------------------------------------------
            double raw = lon_b - lon_a;
            double sep = raw;
            while (sep > 180.0)  sep -= 360.0;
            while (sep < -180.0) sep += 360.0;

            // ------------------------------------------------------------
            // Find closest aspect type + signed delta
            // ------------------------------------------------------------
            double delta = 0.0;
            AeAspectType type = ae_find_closest_aspect(sep, &delta);

            if (type == AE_ASPECT_NONE)
                continue;

            // ------------------------------------------------------------
            // Orb filtering using your aspect_rules logic
            // ------------------------------------------------------------
            double orb_allowed = ae_aspect_orb(a, b, type);
            double orb = fabs(delta);

            if (orb > orb_allowed)
                continue;

            // ------------------------------------------------------------
            // Build aspect record
            // ------------------------------------------------------------
            AeAspect asp;
            asp.a        = a;
            asp.b        = b;
            asp.type     = type;
            asp.orb      = orb;
            asp.delta    = delta; 
            asp.applying = ae_is_applying(lon_a, speed_a, lon_b, speed_b, type);

            // ------------------------------------------------------------
            // Store
            // ------------------------------------------------------------
            if (chart->aspect_count < AE_MAX_ASPECTS) {
                chart->aspects[chart->aspect_count++] = asp;
            }
        }
    }

    return 0;
}

