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

static const char* body_names[] = {
    "Sun","Moon","Mercury","Venus","Mars",
    "Jupiter","Saturn","Uranus","Neptune","Pluto",
    "Node","Lilith","Chiron","Fortune","Vertex",
    "ASC","MC","DSC","IC"
};

int main()
{
    char err[256];

    // Ma Elena — 1957-03-04 18:00 CST (= 1957-03-05 00:00 UTC), Mexico City
    AeInput in;
    ae_init_input(&in);
    in.year = 1957;
    in.month = 3;
    in.day = 5;
    in.hour = 0;
    in.minute = 0;
    in.second = 0;
    in.timezone = 0.0;
    in.lat = 19.4285;
    in.lon = -99.1277;
    in.house_system = 'P';

    AeChartCore chart;
    ae_init_chart_core(&chart);

    double jd_ut = 0.0;
    double lst_deg = 0.0;

    if (ae_compute_metadata(&in, &jd_ut, &lst_deg, err, sizeof(err)) != 0) {
        std::cerr << "Metadata error: " << err << "\n";
        return 1;
    }

    chart.jd_ut = jd_ut;
    chart.lst   = lst_deg;

    AeMetadataCore meta;
    meta.jd_ut   = jd_ut;
    meta.lst_deg = lst_deg;
    meta.lat     = in.lat;
    meta.lon     = in.lon;

    if (ae_compute_core_bodies(jd_ut, &chart, err, sizeof(err)) != 0) {
        std::cerr << "Bodies error: " << err << "\n";
        return 1;
    }

    AeHouseAnglesCore angles;
    AeHouseCuspsCore cusps;

    if (ae_compute_houses(&meta, AE_HOUSE_PLACIDUS, &angles, &cusps, err, sizeof(err)) != 0) {
        std::cerr << "Houses error: " << err << "\n";
        return 1;
    }

    for (int i = 0; i < 12; i++) {
        int house_num = cusps.cusps[i].house;
        double lon    = cusps.cusps[i].longitude;
        if (house_num >= 1 && house_num <= 12)
            chart.houses.cusp[house_num] = lon;
    }

    chart.bodies[AE_BODY_ASC].lon = angles.asc.longitude;
    chart.bodies[AE_BODY_MC].lon  = angles.mc.longitude;
    chart.bodies[AE_BODY_DSC].lon = angles.dsc.longitude;
    chart.bodies[AE_BODY_IC].lon  = angles.ic.longitude;

    if (ae_compute_fortune(&angles,
                           &chart.bodies[AE_BODY_SUN],
                           &chart.bodies[AE_BODY_MOON],
                           &chart.bodies[AE_BODY_FORTUNE]) != 0) {
        std::cerr << "Fortune error\n";
        return 1;
    }

    if (ae_copy_vertex(&angles, &chart.bodies[AE_BODY_VERTEX]) != 0) {
        std::cerr << "Vertex error\n";
        return 1;
    }

    int max_body_names = static_cast<int>(sizeof(body_names) / sizeof(body_names[0]));
    int body_limit = std::min((int)AE_BODY_COUNT, max_body_names);

    std::cout << "\n=== Ma Elena Chart (1957-03-05 00:00 UTC, Mexico City) ===\n";
    std::cout << "\n=== PLANETS ===\n";
    for (int i = 0; i < body_limit; i++) {
        std::cout << std::setw(8) << body_names[i]
                  << "  lon=" << std::fixed << std::setprecision(6)
                  << chart.bodies[i].lon
                  << "\n";
    }

    std::cout << "\n=== HOUSES ===\n";
    for (int h = 1; h <= 12; h++) {
        std::cout << "House " << h << ": " << chart.houses.cusp[h] << "\n";
    }

    double sun = chart.bodies[AE_BODY_SUN].lon;
    double dsc = angles.dsc.longitude;
    double asc = angles.asc.longitude;
    bool is_day = (dsc < asc)
        ? (sun > dsc && sun < asc)
        : (sun > dsc || sun < asc);

    std::cout << "\n=== SECT CHECK ===\n";
    std::cout << "Sun: " << sun << "\n";
    std::cout << "ASC: " << asc << "\n";
    std::cout << "DSC: " << dsc << "\n";
    std::cout << "Sect: " << (is_day ? "DAY chart" : "NIGHT chart") << "\n";
    std::cout << "Fortune: " << chart.bodies[AE_BODY_FORTUNE].lon << "\n";

    double fortune = chart.bodies[AE_BODY_FORTUNE].lon;
    double sign_deg = fmod(fortune, 30.0);
    int sign_num = (int)(fortune / 30.0);
    const char* signs[] = {"Aries","Taurus","Gemini","Cancer","Leo","Virgo",
                           "Libra","Scorpio","Sagittarius","Capricorn","Aquarius","Pisces"};
    std::cout << "Fortune sign: " << signs[sign_num] << " " << std::fixed << std::setprecision(2) << sign_deg << "\n";

    // Expected: DAY chart, Fortune = Libra ~11-12
    bool pass = is_day && sign_num == 6 && sign_deg >= 10.0 && sign_deg <= 13.0;
    std::cout << "\nResult: " << (pass ? "PASS" : "FAIL") << "\n";

    return pass ? 0 : 1;
}
