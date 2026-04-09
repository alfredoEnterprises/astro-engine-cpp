#ifndef AE_POINTS_H
#define AE_POINTS_H

#include "data_types.h"
#include "houses.h"

// Compute Part of Fortune (day/night formula)
int ae_compute_fortune(
    const AeHouseAnglesCore *angles,
    const AeBodyState *sun,
    const AeBodyState *moon,
    AeBodyState *fortune_out
);

// Copy Vertex from houses into chart
int ae_copy_vertex(
    const AeHouseAnglesCore *angles,
    AeBodyState *vertex_out
);

#endif
