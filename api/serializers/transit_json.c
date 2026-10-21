#include "transit_json.h"
#include <stdio.h>
#include <stdarg.h>

#define AE_TJ_JD   "%.6f"
#define AE_TJ_LON  "%.4f"
#define AE_TJ_SPD  "%.5f"

static int append(char *out, size_t out_size, size_t *pos, const char *fmt, ...) {
    if (*pos >= out_size) return -1;
    va_list args;
    va_start(args, fmt);
    int written = vsnprintf(out + *pos, out_size - *pos, fmt, args);
    va_end(args);
    if (written < 0) return -1;
    if (*pos + (size_t)written >= out_size) return -1;
    *pos += (size_t)written;
    return 0;
}

// Transiting body names, index-aligned with AE_BODY_SUN(0)..AE_BODY_CHIRON(12).
static const char *transit_body_name(int i) {
    static const char *names[AE_TRANSIT_BODY_COUNT] = {
        "Sun","Moon","Mercury","Venus","Mars","Jupiter","Saturn",
        "Uranus","Neptune","Pluto","Node","Lilith","Chiron"
    };
    if (i < 0 || i >= AE_TRANSIT_BODY_COUNT) return "Unknown";
    return names[i];
}

int ae_transit_window_to_json(const AeTransitWindow *w, char *out, size_t out_size) {
    if (!w || !out || out_size == 0) return -1;
    size_t pos = 0;

    if (append(out, out_size, &pos,
            "{\"start_jd_ut\":" AE_TJ_JD ",\"day_count\":%d,\"days\":[",
            w->start_jd_ut, w->day_count) != 0) return -1;

    for (int d = 0; d < w->day_count; d++) {
        if (append(out, out_size, &pos, "%s{\"jd_ut\":" AE_TJ_JD ",\"bodies\":{",
                   d ? "," : "", w->days[d].jd_ut) != 0) return -1;
        for (int b = 0; b < AE_TRANSIT_BODY_COUNT; b++) {
            const AeTransitBodyState *s = &w->days[d].bodies[b];
            if (append(out, out_size, &pos,
                    "%s\"%s\":{\"lon\":" AE_TJ_LON ",\"speed\":" AE_TJ_SPD ",\"retro\":%d}",
                    b ? "," : "", transit_body_name(b),
                    s->lon, s->speed_lon, s->retrograde) != 0) return -1;
        }
        if (append(out, out_size, &pos, "}}") != 0) return -1;
    }

    if (append(out, out_size, &pos, "],\"eclipses\":[") != 0) return -1;
    for (int i = 0; i < w->eclipse_count; i++) {
        const AeEclipseEvent *e = &w->eclipses[i];
        if (append(out, out_size, &pos,
                "%s{\"kind\":\"%s\",\"jd_ut\":" AE_TJ_JD ",\"lon\":" AE_TJ_LON ",\"type_flags\":%d}",
                i ? "," : "",
                e->kind == AE_ECLIPSE_SOLAR ? "solar" : "lunar",
                e->jd_ut_max, e->lon, e->type_flags) != 0) return -1;
    }
    if (append(out, out_size, &pos, "]}") != 0) return -1;

    return (int)pos;
}
