#include <cstdio>
#include <cstring>
#include "validation.h"
#include "transits.h"

static int failures = 0;
#define CHECK(cond, msg) do { \
    if (!(cond)) { std::printf("FAIL: %s\n", msg); failures++; } \
    else         { std::printf("ok:   %s\n", msg); } \
} while (0)

int main() {
    char err[256] = {0};

    const char *ok_json =
        "{\"operation\":\"transit-window\",\"start\":"
        "{\"year\":2024,\"month\":4,\"day\":1,\"hour\":12,\"minute\":0,\"second\":0},"
        "\"days\":90}";

    CHECK(ae_request_is_transit_window(ok_json), "detects transit-window op");

    const char *chart_json =
        "{\"year\":1990,\"month\":1,\"day\":1,\"hour\":0,\"minute\":0,\"second\":0,"
        "\"timezone\":0,\"lat\":0,\"lon\":0,\"house_system\":\"P\"}";
    CHECK(!ae_request_is_transit_window(chart_json), "chart request not detected as transit");

    AeTransitRequest req;
    std::memset(&req, 0, sizeof(req));
    bool ok = ae_validate_transit_window_request(ok_json, &req, err, sizeof(err));
    CHECK(ok, "valid transit request accepted");
    CHECK(req.year == 2024 && req.month == 4 && req.day == 1, "start date parsed");
    CHECK(req.hour == 12, "start hour parsed");
    CHECK(req.days == 90, "days parsed");

    // days out of range rejected.
    const char *bad_json =
        "{\"operation\":\"transit-window\",\"start\":"
        "{\"year\":2024,\"month\":4,\"day\":1},\"days\":500}";
    AeTransitRequest bad;
    std::memset(&bad, 0, sizeof(bad));
    bool bad_ok = ae_validate_transit_window_request(bad_json, &bad, err, sizeof(err));
    CHECK(!bad_ok, "days out of range rejected");

    if (failures) { std::printf("\n%d CHECK(S) FAILED\n", failures); return 1; }
    std::printf("\nALL CHECKS PASSED\n");
    return 0;
}
