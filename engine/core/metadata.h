#ifndef AE_METADATA_H
#define AE_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "data_types.h"

// Compute:
// - UT hour
// - Julian Day UT
// - Local Sidereal Time (degrees)
int ae_compute_metadata(
    const AeInput *in,
    double *jd_ut,
    double *lst_deg,
    char *err_msg,
    int err_msg_size
);

#ifdef __cplusplus
}
#endif

#endif
