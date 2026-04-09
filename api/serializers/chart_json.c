#include "chart_json.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Precision rules:
// - jd_ut, lst → 3 decimals
// - all angles, distances, speeds, houses, aspects → 1 decimal
#define AE_JSON_FLOAT_META "%.3f"
#define AE_JSON_FLOAT_ANGLE "%.1f"

// Safe append helper
static int append(char *out, size_t out_size, size_t *pos, const char *fmt, ...) {
    if (*pos >= out_size) return -1;

    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(out + *pos, out_size - *pos, fmt, args);
    va_end(args);

    if (written < 0) return -1;
    if (*pos + written >= out_size) return -1;

    *pos += written;
    return 0;
}

static const char* body_name(AeBodyId id) {
    switch (id) {
        case AE_BODY_SUN:     return "Sun";
        case AE_BODY_MOON:    return "Moon";
        case AE_BODY_MERCURY: return "Mercury";
        case AE_BODY_VENUS:   return "Venus";
        case AE_BODY_MARS:    return "Mars";
        case AE_BODY_JUPITER: return "Jupiter";
        case AE_BODY_SATURN:  return "Saturn";
        case AE_BODY_URANUS:  return "Uranus";
        case AE_BODY_NEPTUNE: return "Neptune";
        case AE_BODY_PLUTO:   return "Pluto";
        case AE_BODY_NODE:    return "Node";
        case AE_BODY_LILITH:  return "Lilith";
        case AE_BODY_CHIRON:  return "Chiron";
        case AE_BODY_FORTUNE: return "Fortune";
        case AE_BODY_VERTEX:  return "Vertex";
        case AE_BODY_ASC:     return "ASC";
        case AE_BODY_MC:      return "MC";
        case AE_BODY_DSC:     return "DSC";
        case AE_BODY_IC:      return "IC";
        default:              return "Unknown";
    }
}

int ae_chartcore_to_json(const AeChartCore *c, char *out, size_t out_size) {
    if (!c || !out || out_size == 0) return -1;

    size_t pos = 0;

    // Start object
    if (append(out, out_size, &pos, "{") != 0) return -1;

    // Metadata (3 decimals)
    if (append(out, out_size, &pos,
        "\"jd_ut\":" AE_JSON_FLOAT_META ","
        "\"lst\":" AE_JSON_FLOAT_META ",",
        c->jd_ut, c->lst) != 0) return -1;

    // Bodies
    if (append(out, out_size, &pos, "\"bodies\":{") != 0) return -1;

    for (int i = 0; i < AE_BODY_COUNT; i++) {
        const AeBodyState *b = &c->bodies[i];
        const char *name = body_name((AeBodyId)i);

        if (append(out, out_size, &pos,
            "\"%s\":{"
                "\"lon\":" AE_JSON_FLOAT_ANGLE ","
                "\"lat\":" AE_JSON_FLOAT_ANGLE ","
                "\"dist\":" AE_JSON_FLOAT_ANGLE ","
                "\"speed_lon\":" AE_JSON_FLOAT_ANGLE ","
                "\"house\":%d,"
                "\"retrograde\":%d"
            "}%s",
            name,
            b->lon, b->lat, b->dist, b->speed_lon,
            b->house, b->retrograde,
            (i < AE_BODY_COUNT - 1 ? "," : "")
        ) != 0) return -1;
    }

    if (append(out, out_size, &pos, "},") != 0) return -1;

    // Houses (1 decimal)
    if (append(out, out_size, &pos, "\"houses\":{") != 0) return -1;

    for (int i = 1; i <= 12; i++) {
        if (append(out, out_size, &pos,
            "\"%d\":" AE_JSON_FLOAT_ANGLE "%s",
            i, c->houses.cusp[i],
            (i < 12 ? "," : "")
        ) != 0) return -1;
    }

    if (append(out, out_size, &pos, "},") != 0) return -1;

    // Aspects (1 decimal)
    if (append(out, out_size, &pos, "\"aspects\":[") != 0) return -1;

    for (int i = 0; i < c->aspect_count; i++) {
        const AeAspect *a = &c->aspects[i];

        if (append(out, out_size, &pos,
            "{"
                "\"a\":\"%s\","
                "\"b\":\"%s\","
                "\"type\":%d,"
                "\"delta\":" AE_JSON_FLOAT_ANGLE ","
                "\"orb\":" AE_JSON_FLOAT_ANGLE ","
                "\"applying\":%d"
            "}%s",
            body_name(a->a),
            body_name(a->b),
            a->type,
            a->delta,
            a->orb,
            a->applying,
            (i < c->aspect_count - 1 ? "," : "")
        ) != 0) return -1;
    }

    if (append(out, out_size, &pos, "]") != 0) return -1;

    // End object
    if (append(out, out_size, &pos, "}") != 0) return -1;

    return (int)pos;
}
