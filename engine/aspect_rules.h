#ifndef AE_ASPECT_RULES_H
#define AE_ASPECT_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "data_types.h"

typedef enum {
    AE_CLASS_LUMINARY = 0,
    AE_CLASS_PERSONAL,
    AE_CLASS_SOCIAL,
    AE_CLASS_OUTER,
    AE_CLASS_ANGLE,
    AE_CLASS_POINT
} AeBodyClass;

extern const double AE_ASPECT_ANGLE[5];

bool ae_aspect_allowed(int body_a, int body_b);
double ae_aspect_orb(int body_a, int body_b, AeAspectType type);

#ifdef __cplusplus
}
#endif

#endif
