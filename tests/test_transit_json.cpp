#include <cstdio>
#include <cstring>
#include "swephexp.h"
#include "transits.h"
#include "transit_json.h"

static int failures = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { std::printf("FAIL: %s\n", msg); failures++; } \
    else         { std::printf("ok:   %s\n", msg); } \
} while (0)

int main() {
    char err[256] = {0};
    double start = swe_julday(2024, 4, 1, 12.0, SE_GREG_CAL);

    AeTransitWindow w;
    std::memset(&w, 0, sizeof(w));
    ae_compute_transit_window(start, 3, &w, err, sizeof(err));
    ae_find_eclipses(start, 3, &w, err, sizeof(err));

    char buf[131072];
    int n = ae_transit_window_to_json(&w, buf, sizeof(buf));
    CHECK(n > 0, "serializer returns positive length");
    CHECK(std::strstr(buf, "\"day_count\":3") != nullptr, "contains day_count:3");
    CHECK(std::strstr(buf, "\"days\":[") != nullptr, "contains days array");
    CHECK(std::strstr(buf, "\"Sun\":{") != nullptr, "contains Sun body");
    CHECK(std::strstr(buf, "\"eclipses\":[") != nullptr, "contains eclipses array");
    // Balanced-ish: first char '{' and last non-null '}'.
    CHECK(buf[0] == '{', "starts with object brace");
    CHECK(buf[n - 1] == '}', "ends with object brace");

    if (failures) { std::printf("\n%d CHECK(S) FAILED\n", failures); return 1; }
    std::printf("\nALL CHECKS PASSED\n");
    return 0;
}
