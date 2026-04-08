#include <iostream>

extern "C" {
#include "engine.h"
#include "data_types.h"
#include "metadata.h"
#include "bodies.h"
#include "houses.h"
#include "swephexp.h"
}

int main() {
    // ----------------------------------------
    // Swiss Ephemeris init
    // ----------------------------------------
    ae_init_swiss("/Users/alfredo/Documents/projects/astro-engine-cpp/external/swisseph/ephe");
    std::cout << "DEBUG: after ae_init_swiss\n";

    // ----------------------------------------
    // Input
    // ----------------------------------------
    AeInput in{};
    in.year = 1984;
    in.month = 3;
    in.day = 12;
    in.hour = 4;
    in.minute = 0;
    in.second = 0;
    in.timezone = 0.0;
    in.lat = 40.7128;
    in.lon = -74.0060;
    in.house_system = 'P';

    char err[256];

    // ----------------------------------------
    // METADATA TEST
    // ----------------------------------------
    double jd_ut = 0.0;
    double lst_deg = 0.0;

    int rc1 = ae_compute_metadata(&in, &jd_ut, &lst_deg, err, sizeof(err));
    std::cout << "=== TEST: METADATA ===\n";
    std::cout << "JD(UT): " << jd_ut << "\n";
    std::cout << "LST (deg): " << lst_deg << "\n";

    // ----------------------------------------
    // BODIES TEST
    // ----------------------------------------
    AeChartCore chart{};
    ae_init_chart_core(&chart);

    int rc2 = ae_compute_core_bodies(jd_ut, &chart, err, sizeof(err));

    std::cout << "=== TEST: BODIES (ALL CORE MOVING BODIES) ===\n";
    std::cout << "Sun: " << chart.bodies[
AE_BODY_SUN].lon
              << "  (speed=" << chart.bodies[
AE_BODY_SUN].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_SUN].retrograde << ")\n";

    std::cout << "Moon: " << chart.bodies[
AE_BODY_MOON].lon
              << "  (speed=" << chart.bodies[
AE_BODY_MOON].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_MOON].retrograde << ")\n";

    std::cout << "Mercury: " << chart.bodies[
AE_BODY_MERCURY].lon
              << "  (speed=" << chart.bodies[
AE_BODY_MERCURY].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_MERCURY].retrograde << ")\n";

    std::cout << "Venus: " << chart.bodies[
AE_BODY_VENUS].lon
              << "  (speed=" << chart.bodies[
AE_BODY_VENUS].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_VENUS].retrograde << ")\n";

    std::cout << "Mars: " << chart.bodies[
AE_BODY_MARS].lon
              << "  (speed=" << chart.bodies[
AE_BODY_MARS].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_MARS].retrograde << ")\n";

    std::cout << "Jupiter: " << chart.bodies[
AE_BODY_JUPITER].lon
              << "  (speed=" << chart.bodies[
AE_BODY_JUPITER].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_JUPITER].retrograde << ")\n";

    std::cout << "Saturn: " << chart.bodies[
AE_BODY_SATURN].lon
              << "  (speed=" << chart.bodies[
AE_BODY_SATURN].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_SATURN].retrograde << ")\n";

    std::cout << "Uranus: " << chart.bodies[
AE_BODY_URANUS].lon
              << "  (speed=" << chart.bodies[
AE_BODY_URANUS].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_URANUS].retrograde << ")\n";

    std::cout << "Neptune: " << chart.bodies[
AE_BODY_NEPTUNE].lon
              << "  (speed=" << chart.bodies[
AE_BODY_NEPTUNE].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_NEPTUNE].retrograde << ")\n";

    std::cout << "Pluto: " << chart.bodies[
AE_BODY_PLUTO].lon
              << "  (speed=" << chart.bodies[
AE_BODY_PLUTO].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_PLUTO].retrograde << ")\n";

    std::cout << "True Node: " << chart.bodies[
AE_BODY_NODE].lon
              << "  (speed=" << chart.bodies[
AE_BODY_NODE].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_NODE].retrograde << ")\n";

    std::cout << "Lilith (Mean Apogee): " << chart.bodies[
AE_BODY_LILITH].lon
              << "  (speed=" << chart.bodies[
AE_BODY_LILITH].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_LILITH].retrograde << ")\n";

    std::cout << "Chiron: " << chart.bodies[
AE_BODY_CHIRON].lon
              << "  (speed=" << chart.bodies[
AE_BODY_CHIRON].speed_lon
              << ", retro=" << chart.bodies[
AE_BODY_CHIRON].retrograde << ")\n";

    // ----------------------------------------
    // HOUSES TEST
    // ----------------------------------------

    // Construct metadata struct for houses
    AeMetadataCore meta{};
    meta.jd_ut = jd_ut;
    meta.lst_deg = lst_deg;
    meta.lat = in.lat;
    meta.lon = in.lon;

    AeHouseAnglesCore angles{};
    AeHouseCuspsCore cusps{};

    int rc3 = ae_compute_houses(&meta, AE_HOUSE_PLACIDUS, &angles, &cusps, err, sizeof(err));

    std::cout << "=== TEST: HOUSES (PLACIDUS) ===\n";
    std::cout << "ASC: " << angles.asc.longitude << "\n";
    std::cout << "MC: " << angles.mc.longitude << "\n";
    std::cout << "DSC: " << angles.dsc.longitude << "\n";
    std::cout << "IC: " << angles.ic.longitude << "\n";
    std::cout << "Vertex: " << angles.vertex.longitude << "\n";
    std::cout << "ARMC: " << angles.armc.longitude << "\n";
    std::cout << "Obliquity: " << angles.obliquity << "\n";

    std::cout << "--- Cusps ---\n";
    for (int i = 0; i < 12; i++) {
        std::cout << "House " << (i + 1) << ": " << cusps.cusps[i].longitude << "\n";
    }

    return 0;
}
