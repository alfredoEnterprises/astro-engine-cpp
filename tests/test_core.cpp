#include <iostream>
#include <iomanip>
#include <cstring>

extern "C" {
#include "chart_core.h"
}

// ------------------------------------------------------------
// Local helper: body names (since engine does not expose one)
// ------------------------------------------------------------
static const char* body_name(int i) {
    switch (i) {
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

        case AE_BODY_ASC:     return "ASC";
        case AE_BODY_MC:      return "MC";
        case AE_BODY_DSC:     return "DSC";
        case AE_BODY_IC:      return "IC";

        case AE_BODY_FORTUNE: return "Fortune";
        case AE_BODY_VERTEX:  return "Vertex";

        default: return "Unknown";
    }
}

// ------------------------------------------------------------
// Pretty printer for AeChartCore
// ------------------------------------------------------------
static void print_chart(const AeChartCore &chart) {
    std::cout << "\n=== CHART RESULTS ===\n";

    std::cout << "JD_UT:  " << chart.jd_ut << "\n";
    std::cout << "LST:    " << chart.lst << "°\n\n";

    std::cout << "--- BODIES & POINTS ---\n";
    for (int i = 0; i < AE_BODY_COUNT; i++) {
        std::cout << std::setw(12) << body_name(i)
                  << ": " << std::fixed << std::setprecision(6)
                  << chart.bodies[i].lon << "°"
                  << "  lat=" << chart.bodies[i].lat
                  << "  dist=" << chart.bodies[i].dist
                  << "\n";
    }

    std::cout << "\n--- HOUSES ---\n";
    for (int i = 1; i <= 12; i++) {
        std::cout << "House " << i << ": "
                  << chart.houses.cusp[i] << "°\n";
    }

    std::cout << "\n--- ASPECTS ---\n";
    for (int i = 0; i < chart.aspect_count; i++) {
        const AeAspect &a = chart.aspects[i];
        std::cout << body_name(a.a) << " - "
                  << body_name(a.b)
                  << "  type=" << a.type
                  << "  orb=" << a.orb
                  << "  delta=" << a.delta
                  << "  applying=" << a.applying
                  << "\n";
    }

    std::cout << "\n======================\n\n";
}

// ------------------------------------------------------------
// VALID TEST
// ------------------------------------------------------------
static void run_valid_test() {
    std::cout << "=== VALID INPUT TEST ===\n";

    AeInput in{};
    in.year = 1983;
    in.month = 3;
    in.day = 28;
    in.hour = 8;
    in.minute = 45;
    in.second = 0;
    in.timezone = -6.0;
    in.lat = 19.433333;
    in.lon = -99.133333;
    in.house_system = 'P'; // Placidus

    AeChartCore out{};
    char err[256];
    std::memset(err, 0, sizeof(err));

    int rc = ae_compute_chart_core(&in, &out, err, sizeof(err));

    if (rc != 0) {
        std::cout << "ERROR: " << err << "\n";
        return;
    }

    print_chart(out);
}

// ------------------------------------------------------------
// INVALID TEST WRAPPER
// ------------------------------------------------------------
static void run_invalid_test(const char *label, const AeInput &in) {
    std::cout << "=== INVALID TEST: " << label << " ===\n";

    AeChartCore out{};
    char err[256];
    std::memset(err, 0, sizeof(err));

    int rc = ae_compute_chart_core(&in, &out, err, sizeof(err));

    if (rc == 0) {
        std::cout << "UNEXPECTED SUCCESS — should have failed\n";
        print_chart(out);
    } else {
        std::cout << "Expected error: " << err << "\n\n";
    }
}

// ------------------------------------------------------------
// INVALID TEST SUITE
// ------------------------------------------------------------
static void run_invalid_tests() {
    // 1. Bad latitude
    {
        AeInput in{};
        in.year = 2000; in.month = 1; in.day = 1;
        in.hour = 12; in.minute = 0; in.second = 0;
        in.timezone = 0;
        in.lat = 200.0; // invalid
        in.lon = 0.0;
        in.house_system = 'P';
        run_invalid_test("Bad latitude", in);
    }

    // 2. Bad longitude
    {
        AeInput in{};
        in.year = 2000; in.month = 1; in.day = 1;
        in.hour = 12; in.minute = 0; in.second = 0;
        in.timezone = 0;
        in.lat = 0.0;
        in.lon = -500.0; // invalid
        in.house_system = 'P';
        run_invalid_test("Bad longitude", in);
    }

    // 3. Bad timezone
    {
        AeInput in{};
        in.year = 2000; in.month = 1; in.day = 1;
        in.hour = 12; in.minute = 0; in.second = 0;
        in.timezone = -99.0; // invalid
        in.lat = 0.0;
        in.lon = 0.0;
        in.house_system = 'P';
        run_invalid_test("Bad timezone", in);
    }

    // 4. Invalid house system
    {
        AeInput in{};
        in.year = 2000; in.month = 1; in.day = 1;
        in.hour = 12; in.minute = 0; in.second = 0;
        in.timezone = 0;
        in.lat = 0.0;
        in.lon = 0.0;
        in.house_system = 'Z'; // invalid
        run_invalid_test("Invalid house system", in);
    }
}

// ------------------------------------------------------------
// MAIN
// ------------------------------------------------------------
int main() {
    run_valid_test();
    run_invalid_tests();
    return 0;
}
