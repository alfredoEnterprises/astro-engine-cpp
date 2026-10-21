#ifndef AE_TRANSIT_JSON_H
#define AE_TRANSIT_JSON_H

#include <stddef.h>
#include "transits.h"

#ifdef __cplusplus
extern "C" {
#endif

// Serialize a transit window to JSON into `out`. Returns bytes written (>=0)
// or -1 on overflow/error.
int ae_transit_window_to_json(const AeTransitWindow *w, char *out, size_t out_size);

#ifdef __cplusplus
}
#endif

#endif
