#ifndef AE_JSON_SERIALIZER_H
#define AE_JSON_SERIALIZER_H

#include "../../engine/core/data_types.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Serialize AeChartCore into JSON.
// Returns number of bytes written (excluding null terminator).
// If buffer is too small, returns -1.
int ae_chartcore_to_json(const AeChartCore *c, char *out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif
