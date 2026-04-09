#ifndef AE_ASPECTS_H
#define AE_ASPECTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "data_types.h"
#include "aspect_rules.h"

#define AE_MAX_ASPECTS 512

AeAspectType ae_find_closest_aspect(double separation_deg, double *out_delta);

bool ae_is_applying(
    double lon_a, double speed_a,
    double lon_b, double speed_b,
    AeAspectType type
);

int ae_compute_aspects(AeChartCore *chart);

#ifdef __cplusplus
}
#endif

#endif
