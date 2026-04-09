#include <iostream>
#include <cstring>

extern "C" {
#include "../api/validation/validation.h"
#include "../engine/core/data_types.h"
}

static void run_valid_test() {
    std::cout << "=== VALIDATION: VALID INPUT TEST ===\n";

    const char *json =
        "{"
        "\"year\":1983,"
        "\"month\":3,"
        "\"day\":28,"
        "\"hour\":8,"
        "\"minute\":45,"
        "\"second\":0,"
        "\"timezone\":-6.0,"
        "\"lat\":19.433333,"
        "\"lon\":-99.133333,"
        "\"house_system\":\"P\""
        "}";

    AeInput in{};
    char err[256];
    memset(err, 0, sizeof(err));

    bool ok = ae_validate_chart_request(json, &in, err, sizeof(err));

    if (!ok) {
        std::cout << "FAILED (unexpected): " << err << "\n\n";
        return;
    }

    std::cout << "PASSED — Parsed successfully\n";
    std::cout << "Parsed values:\n";
    std::cout << "  year=" << in.year << "\n";
    std::cout << "  month=" << in.month << "\n";
    std::cout << "  day=" << in.day << "\n";
    std::cout << "  hour=" << in.hour << "\n";
    std::cout << "  minute=" << in.minute << "\n";
    std::cout << "  second=" << in.second << "\n";
    std::cout << "  timezone=" << in.timezone << "\n";
    std::cout << "  lat=" << in.lat << "\n";
    std::cout << "  lon=" << in.lon << "\n";
    std::cout << "  house_system=" << in.house_system << "\n\n";
}

static void run_invalid_test(const char *label, const char *json) {
    std::cout << "=== VALIDATION: INVALID TEST (" << label << ") ===\n";

    AeInput in{};
    char err[256];
    memset(err, 0, sizeof(err));

    bool ok = ae_validate_chart_request(json, &in, err, sizeof(err));

    if (ok) {
        std::cout << "FAILED — Should have rejected input\n\n";
        return;
    }

    std::cout << "PASSED — Error: " << err << "\n\n";
}

int main() {
    run_valid_test();

    // Invalid: missing field
    run_invalid_test("missing year",
        "{"
        "\"month\":3,"
        "\"day\":28,"
        "\"hour\":8,"
        "\"minute\":45,"
        "\"second\":0,"
        "\"timezone\":-6.0,"
        "\"lat\":19.4,"
        "\"lon\":-99.1,"
        "\"house_system\":\"P\""
        "}"
    );

    // Invalid: bad latitude
    run_invalid_test("bad latitude",
        "{"
        "\"year\":1983,"
        "\"month\":3,"
        "\"day\":28,"
        "\"hour\":8,"
        "\"minute\":45,"
        "\"second\":0,"
        "\"timezone\":-6.0,"
        "\"lat\":200.0,"
        "\"lon\":-99.1,"
        "\"house_system\":\"P\""
        "}"
    );

    // Invalid: house system too long
    run_invalid_test("bad house system",
        "{"
        "\"year\":1983,"
        "\"month\":3,"
        "\"day\":28,"
        "\"hour\":8,"
        "\"minute\":45,"
        "\"second\":0,"
        "\"timezone\":-6.0,"
        "\"lat\":19.4,"
        "\"lon\":-99.1,"
        "\"house_system\":\"Plac\""
        "}"
    );

    return 0;
}
