#ifndef AE_API_VALIDATION_H
#define AE_API_VALIDATION_H

#include "../../engine/core/data_types.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Validate a JSON request string and convert it into AeInput.
// Returns true if valid, false if invalid.
// On failure, `err` will contain a human-readable error message.
bool ae_validate_chart_request(
    const char *json,
    AeInput *out,
    char *err,
    size_t err_size
);

#ifdef __cplusplus
}
#endif

#endif
