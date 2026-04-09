#include "aspect_rules.h"
#include "bodies.h"     // for AE_BODY_* indices
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

// ------------------------------------------------------------
// ASPECT ANGLES (0, 60, 90, 120, 180)
// ------------------------------------------------------------
const double AE_ASPECT_ANGLE[5] = {
    0.0,    // Conjunction
    180.0,  // Opposition
    120.0,  // Trine
    90.0,   // Square
    60.0    // Sextile
};


// ------------------------------------------------------------
// BODY CLASSIFICATION
// ------------------------------------------------------------
AeBodyClass ae_body_class(int body_index)
{
    switch (body_index) {
        case AE_BODY_SUN:
        case AE_BODY_MOON:
            return AE_CLASS_LUMINARY;

        case AE_BODY_MERCURY:
        case AE_BODY_VENUS:
        case AE_BODY_MARS:
            return AE_CLASS_PERSONAL;

        case AE_BODY_JUPITER:
        case AE_BODY_SATURN:
            return AE_CLASS_SOCIAL;

        case AE_BODY_URANUS:
        case AE_BODY_NEPTUNE:
        case AE_BODY_PLUTO:
            return AE_CLASS_OUTER;

        case AE_BODY_NODE:
        case AE_BODY_LILITH:
        case AE_BODY_CHIRON:
        case AE_BODY_FORTUNE:
        case AE_BODY_VERTEX:
            return AE_CLASS_POINT;

        case AE_BODY_ASC:
        case AE_BODY_MC:
        case AE_BODY_DSC:
        case AE_BODY_IC:
            return AE_CLASS_ANGLE;

        default:
            return AE_CLASS_POINT;
    }
}

// ------------------------------------------------------------
// BASE ORBS (Astro-Seek style)
// ------------------------------------------------------------
double ae_base_orb(AeBodyClass cls)
{
    switch (cls) {
        case AE_CLASS_LUMINARY: return 10.0;
        case AE_CLASS_ANGLE:    return 9.0;
        case AE_CLASS_PERSONAL: return 7.5;
        case AE_CLASS_SOCIAL:   return 7.5;
        case AE_CLASS_OUTER:    return 6.5;
        case AE_CLASS_POINT:    return 6.5;
        default:                return 6.5;
    }
}

// ------------------------------------------------------------
// ASPECT MULTIPLIERS
// ------------------------------------------------------------
double ae_aspect_multiplier(AeAspectType type)
{
    switch (type) {
        case AE_ASPECT_CONJUNCTION: return 1.00;
        case AE_ASPECT_SEXTILE:     return 0.70;
        case AE_ASPECT_SQUARE:      return 0.90;
        case AE_ASPECT_TRINE:       return 0.90;
        case AE_ASPECT_OPPOSITION:  return 1.00;
        default:                    return 1.00;
    }
}

// ------------------------------------------------------------
// FINAL ORB ALLOWED BETWEEN TWO BODIES
// ------------------------------------------------------------
double ae_aspect_orb(int body_a, int body_b, AeAspectType type)
{
    AeBodyClass ca = ae_body_class(body_a);
    AeBodyClass cb = ae_body_class(body_b);

    double base_a = ae_base_orb(ca);
    double base_b = ae_base_orb(cb);

    double base = fmin(base_a, base_b);
    double mult = ae_aspect_multiplier(type);

    return base * mult;
}

// ------------------------------------------------------------
// INCLUSION RULES (Astro-Seek style)
// ------------------------------------------------------------
bool ae_aspect_allowed(int a, int b)
{
    static int printed = 0;
    if (!printed) {
        printed = 1;
    }

    AeBodyClass ca = ae_body_class(a);
    AeBodyClass cb = ae_body_class(b);

    if (ca == AE_CLASS_ANGLE && cb == AE_CLASS_ANGLE) {
        return false;
    }
    
    if (ca <= AE_CLASS_OUTER && cb <= AE_CLASS_OUTER) {
        return true;
    }
    
    if (ca <= AE_CLASS_OUTER && cb == AE_CLASS_POINT) {
        return true;
    }
    
    if (cb <= AE_CLASS_OUTER && ca == AE_CLASS_POINT) {
        return true;
    }
    
    if (ca <= AE_CLASS_OUTER && cb == AE_CLASS_ANGLE) {
        return true;
    }
    
    if (cb <= AE_CLASS_OUTER && ca == AE_CLASS_ANGLE) {
        return true;
    }
    
    if ((a == AE_BODY_FORTUNE && b == AE_BODY_VENUS) ||
        (b == AE_BODY_FORTUNE && a == AE_BODY_VENUS)) {
        return true;
    }
    
    if ((a == AE_BODY_FORTUNE && (b == AE_BODY_ASC || b == AE_BODY_DSC)) ||
        (b == AE_BODY_FORTUNE && (a == AE_BODY_ASC || a == AE_BODY_DSC))) {
        return true;
    }
    
    if ((a == AE_BODY_VERTEX && b == AE_BODY_MARS) ||
        (b == AE_BODY_VERTEX && a == AE_BODY_MARS)) {
        return true;
    }
    

    return false;
    
}

