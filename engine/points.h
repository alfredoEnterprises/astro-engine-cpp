#ifndef AE_POINTS_H
#define AE_POINTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "data_types.h"
#include "houses.h"

int ae_compute_fortune(
    const AeHouseAnglesCore *angles,
    const AeBodyState *sun,
    const AeBodyState *moon,
    AeBodyState *fortune_out
);

int ae_copy_vertex(
    const AeHouseAnglesCore *angles,
    AeBodyState *vertex_out
);

#ifdef __cplusplus
}
#endif

#endif

