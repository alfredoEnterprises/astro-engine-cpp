#include "data_types.h"
#include "metadata.h"
#include "bodies.h"
#include "houses.h"
#include "points.h"
#include "aspects.h"
#include <stdio.h>
#include <string.h>

// ------------------------------------------------------------
// Helper: write error message safely
// ------------------------------------------------------------
static int ae_set_error(char *err, size_t err_size, const char *msg) {
    if (err && err_size > 0) {
        strncpy(err, msg, err_size - 1);
        err[err_size - 1] = '\0';
    }
    return -1;
}

// ------------------------------------------------------------
// Helper: validate house system
// ------------------------------------------------------------
static int ae_is_valid_house_system(char hs) {
    switch (hs) {
        case 'P': // Placidus
        case 'K': // Koch
        case 'R': // Regiomontanus
        case 'C': // Campanus
        case 'E': // Equal
        case 'W': // Whole sign
            return 1;
        default:
            return 0;
    }
}

// ------------------------------------------------------------
// CENTRAL FUNCTION
// ------------------------------------------------------------
int ae_compute_chart_core(const AeInput *in, AeChartCore *out, char *err, size_t err_size)
{
    if (!in || !out)
        return ae_set_error(err, err_size, "Null pointer input");

    // ------------------------------------------------------------
    // VALIDATION LAYER (domain validation)
    // ------------------------------------------------------------

    if (in->year < 1600 || in->year > 2500)
        return ae_set_error(err, err_size, "Year out of range (1600–2500)");

    if (in->month < 1 || in->month > 12)
        return ae_set_error(err, err_size, "Month out of range (1–12)");

    if (in->day < 1 || in->day > 31)
        return ae_set_error(err, err_size, "Day out of range (1–31)");

    if (in->hour < 0 || in->hour > 23)
        return ae_set_error(err, err_size, "Hour out of range (0–23)");

    if (in->minute < 0 || in->minute > 59)
        return ae_set_error(err, err_size, "Minute out of range (0–59)");

    if (in->second < 0 || in->second > 59)
        return ae_set_error(err, err_size, "Second out of range (0–59)");

    if (in->timezone < -12.0 || in->timezone > 14.0)
        return ae_set_error(err, err_size, "Timezone out of range (-12 to +14)");

    if (in->lat < -90.0 || in->lat > 90.0)
        return ae_set_error(err, err_size, "Latitude out of range (-90 to +90)");

    if (in->lon < -180.0 || in->lon > 180.0)
        return ae_set_error(err, err_size, "Longitude out of range (-180 to +180)");

    if (!ae_is_valid_house_system(in->house_system))
        return ae_set_error(err, err_size, "Invalid house system");

    // ------------------------------------------------------------
    // INITIALIZE OUTPUT STRUCTURE
    // ------------------------------------------------------------
    ae_init_chart_core(out);

    // ------------------------------------------------------------
    // METADATA (JD, LST)
    // ------------------------------------------------------------
    double jd_ut = 0.0;
    double lst_deg = 0.0;

    if (ae_compute_metadata(in, &jd_ut, &lst_deg, err, err_size) != 0)
        return -1;

    out->jd_ut = jd_ut;
    out->lst   = lst_deg;

    // Build metadata core for houses
    AeMetadataCore meta;
    meta.jd_ut   = jd_ut;
    meta.lst_deg = lst_deg;
    meta.lat     = in->lat;
    meta.lon     = in->lon;

    // ------------------------------------------------------------
    // BODIES
    // ------------------------------------------------------------
    if (ae_compute_core_bodies(jd_ut, out, err, err_size) != 0)
        return -1;

    // ------------------------------------------------------------
    // HOUSES
    // ------------------------------------------------------------
    AeHouseAnglesCore angles;
    AeHouseCuspsCore cusps;

    if (ae_compute_houses(&meta, in->house_system, &angles, &cusps, err, err_size) != 0)
        return -1;

    // Copy cusps
    for (int i = 0; i < 12; i++) {
        int h = cusps.cusps[i].house;
        if (h >= 1 && h <= 12)
            out->houses.cusp[h] = cusps.cusps[i].longitude;
    }

    // Copy angles into bodies
    out->bodies[AE_BODY_ASC].lon = angles.asc.longitude;
    out->bodies[AE_BODY_MC].lon  = angles.mc.longitude;
    out->bodies[AE_BODY_DSC].lon = angles.dsc.longitude;
    out->bodies[AE_BODY_IC].lon  = angles.ic.longitude;

    // ------------------------------------------------------------
    // POINTS (Fortune, Vertex)
    // ------------------------------------------------------------
    if (ae_compute_fortune(&angles,
                           &out->bodies[AE_BODY_SUN],
                           &out->bodies[AE_BODY_MOON],
                           &out->bodies[AE_BODY_FORTUNE]) != 0)
        return ae_set_error(err, err_size, "Fortune computation failed");

    if (ae_copy_vertex(&angles, &out->bodies[AE_BODY_VERTEX]) != 0)
        return ae_set_error(err, err_size, "Vertex computation failed");

    // ------------------------------------------------------------
    // ASPECTS
    // ------------------------------------------------------------
    if (ae_compute_aspects(out) != 0)
        return ae_set_error(err, err_size, "Aspect computation failed");

    return 0;
}
