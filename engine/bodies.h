#include "data_types.h"
#ifndef AE_BODIES_H
#define AE_BODIES_H

#ifdef __cplusplus
extern "C" {
#endif



// Compute a single body at given JD(UT)
int ae_compute_body(
    double jd_ut,
    AeBodyId id,
    AeBodyState *out,
    char *err_msg,
    int err_msg_size
);

// Compute core moving bodies (Sun–Pluto, Node, Lilith, Chiron) into chart
int ae_compute_core_bodies(
    double jd_ut,
    AeChartCore *chart,
    char *err_msg,
    int err_msg_size
);

#ifdef __cplusplus
}
#endif

#endif
