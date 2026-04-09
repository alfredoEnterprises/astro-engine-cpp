#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>
#include <algorithm>

#include "data_types.h"
#include "metadata.h"
#include "bodies.h"
#include "houses.h"
#include "points.h"
#include "aspects.h"
#include "aspect_rules.h"

static double norm180(double x)
{
    while (x > 180.0) x -= 360.0;
    while (x < -180.0) x += 360.0;
    return x;
}

static const char* body_names[] = {
    "Sun","Moon","Mercury","Venus","Mars",
    "Jupiter","Saturn","Uranus","Neptune","Pluto",
    "Node","Lilith","Chiron","Fortune","Vertex",
    "ASC","MC","DSC","IC"
};

static const char* aspect_names[] = {
    "Conjunction",
    "Opposition",
    "Trine",
    "Square",
    "Sextile",
    "None"
};

int main()
{
    char err[256];

    // -----------------------------
    // INPUT
    // -----------------------------
    AeInput in;
    ae_init_input(&in);

   /* in.year = 1983;
    in.month = 3;
    in.day = 28;
    in.hour = 8;
    in.minute = 45;
    in.second = 0;
    in.timezone = -6.0;
    in.lat = 19.433333;
    in.lon = -99.133333;
    in.house_system = 'P';*/
    in.year = 1999;
    in.month = 3;
    in.day = 5;
    in.hour = 16;
    in.minute = 3;
    in.second = 0;
    in.timezone = +9.0;
    in.lat = 35.35;        // 35°21' N
    in.lon = 137.183333;   // 137°11' E
    in.house_system = 'P';


    // -----------------------------
    // CHART CORE
    // -----------------------------
    AeChartCore chart;
    ae_init_chart_core(&chart); // should memset/zero everything

    // -----------------------------
    // METADATA
    // -----------------------------
    double jd_ut = 0.0;
    double lst_deg = 0.0;

    if (ae_compute_metadata(&in, &jd_ut, &lst_deg, err, sizeof(err)) != 0) {
        std::cerr << "Metadata error: " << err << "\n";
        return 1;
    }

    chart.jd_ut = jd_ut;
    chart.lst   = lst_deg;

    // Build AeMetadataCore for houses
    AeMetadataCore meta;
    meta.jd_ut   = jd_ut;
    meta.lst_deg = lst_deg;
    meta.lat     = in.lat;
    meta.lon     = in.lon;

    // -----------------------------
    // BODIES
    // -----------------------------
    if (ae_compute_core_bodies(jd_ut, &chart, err, sizeof(err)) != 0) {
        std::cerr << "Bodies error: " << err << "\n";
        return 1;
    }

    // -----------------------------
    // HOUSES
    // -----------------------------
    AeHouseAnglesCore angles;
    AeHouseCuspsCore cusps;

    if (ae_compute_houses(&meta, AE_HOUSE_PLACIDUS, &angles, &cusps, err, sizeof(err)) != 0) {
        std::cerr << "Houses error: " << err << "\n";
        return 1;
    }

    // Copy cusps into chart.houses.cusp[1..12]
    for (int i = 0; i < 12; i++) {
        int house_num = cusps.cusps[i].house;      // 1..12
        double lon    = cusps.cusps[i].longitude;
        if (house_num >= 1 && house_num <= 12) {
            chart.houses.cusp[house_num] = lon;
        }
    }

    // Copy angles into chart bodies
    chart.bodies[AE_BODY_ASC].lon = angles.asc.longitude;
    chart.bodies[AE_BODY_MC].lon  = angles.mc.longitude;
    chart.bodies[AE_BODY_DSC].lon = angles.dsc.longitude;
    chart.bodies[AE_BODY_IC].lon  = angles.ic.longitude;

    // -----------------------------
    // POINTS (Fortune, Vertex)
    // -----------------------------
    if (ae_compute_fortune(
            &angles,
            &chart.bodies[AE_BODY_SUN],
            &chart.bodies[AE_BODY_MOON],
            &chart.bodies[AE_BODY_FORTUNE]) != 0) {
        std::cerr << "Fortune error\n";
        return 1;
    }

    if (ae_copy_vertex(
            &angles,
            &chart.bodies[AE_BODY_VERTEX]) != 0) {
        std::cerr << "Vertex error\n";
        return 1;
    }

    // -----------------------------
    // ASPECTS
    // -----------------------------
    if (ae_compute_aspects(&chart) != 0) {
        std::cerr << "Aspect error\n";
        return 1;
    }

    // -----------------------------
    // PRINT PLANETS
    // -----------------------------
    std::cout << "\n=== PLANETS ===\n";

    int max_body_names = static_cast<int>(sizeof(body_names) / sizeof(body_names[0]));
    int body_limit = std::min((int)AE_BODY_COUNT, max_body_names);


    for (int i = 0; i < body_limit; i++) {
        std::cout << std::setw(8) << body_names[i]
                  << "  lon=" << std::fixed << std::setprecision(6)
                  << chart.bodies[i].lon
                  << "  speed=" << chart.bodies[i].speed_lon
                  << "  house=" << chart.bodies[i].house
                  << "  retro=" << chart.bodies[i].retrograde
                  << "\n";
    }

    // -----------------------------
    // PRINT HOUSES
    // -----------------------------
    std::cout << "\n=== HOUSES ===\n";
    for (int h = 1; h <= 12; h++) {
        std::cout << "House " << h << ": "
                  << chart.houses.cusp[h] << "\n";
    }

    // -----------------------------
    // PRINT ASPECTS
    // -----------------------------
    std::cout << "\n=== ASPECTS ===\n";

    int max_aspects = chart.aspect_count;
    if (max_aspects < 0 || max_aspects > AE_MAX_ASPECTS) {
        std::cerr << "Invalid aspect_count: " << chart.aspect_count << "\n";
        max_aspects = std::max(0, std::min(chart.aspect_count, AE_MAX_ASPECTS));
    }

    int max_aspect_types = static_cast<int>(sizeof(aspect_names) / sizeof(aspect_names[0]));

    for (int i = 0; i < max_aspects; i++) {
        const AeAspect &asp = chart.aspects[i];

        if (asp.a < 0 || asp.a >= body_limit ||
            asp.b < 0 || asp.b >= body_limit) {
            std::cerr << "Invalid aspect body index: a=" << asp.a
                      << " b=" << asp.b << "\n";
            continue;
        }

        if (asp.type < 0 || asp.type >= max_aspect_types) {
            std::cerr << "Invalid aspect type: " << asp.type << "\n";
            continue;
        }

        double lon_a = chart.bodies[asp.a].lon;
        double lon_b = chart.bodies[asp.b].lon;
        double exact = AE_ASPECT_ANGLE[asp.type];

        double delta = asp.delta;


        std::cout
            << body_names[asp.a] << " "
            << aspect_names[asp.type] << " "
            << body_names[asp.b]
            << "  orb=" << std::fixed << std::setprecision(4) << asp.orb
            << "  delta=" << delta
            << "  applying=" << (asp.applying ? "yes" : "no")
            << "\n";
    }

    return 0;
}
