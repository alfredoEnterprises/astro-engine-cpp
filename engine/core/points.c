#include "points.h"
#include <math.h>
#include <stdbool.h>

static double norm(double x) {
    while (x < 0) x += 360.0;
    while (x >= 360.0) x -= 360.0;
    return x;
}

int ae_compute_fortune(
    const AeHouseAnglesCore *angles,
    const AeBodyState *sun,
    const AeBodyState *moon,
    AeBodyState *fortune_out
) {
    if (!angles || !sun || !moon || !fortune_out)
        return -1;

    double asc      = norm(angles->asc.longitude);
    double sun_deg  = norm(sun->lon);
    double moon_deg = norm(moon->lon);
    double dsc      = norm(angles->dsc.longitude);

    // Day chart: Sun above the horizon (between DSC and ASC through MC)
    bool is_day = false;
    if (dsc < asc)
        is_day = (sun_deg > dsc && sun_deg < asc);
    else
        is_day = (sun_deg > dsc || sun_deg < asc);

    double f = is_day
        ? asc + moon_deg - sun_deg
        : asc - moon_deg + sun_deg;

    fortune_out->lon = norm(f);
    return 0;
}


int ae_copy_vertex(
    const AeHouseAnglesCore *angles,
    AeBodyState *vertex_out
) {
    if (!angles || !vertex_out)
        return -1;

    vertex_out->lon = angles->vertex.longitude;
    return 0;
}

