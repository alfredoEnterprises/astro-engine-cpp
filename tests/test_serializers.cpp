#include <stdio.h>
#include <string.h>

extern "C" {
#include "data_types.h"
#include "chart_core.h"
#include "chart_json.h"
}

int main() {
    // Prepare input (Mexico City, 28 March 1983, 08:45 CST, Placidus)
    AeInput in = {0};
    in.year = 1983;
    in.month = 3;
    in.day = 28;
    in.hour = 8;
    in.minute = 45;
    in.second = 0;

    in.timezone = -6.0;        // CST
    in.lat = 19.433333;        // 19°26' N
    in.lon = -99.133333;       // 99°8' W
    in.house_system = 'P';     // Placidus

    AeChartCore chart = {0};
    char err[256];
    memset(err, 0, sizeof(err));

    // Compute chart (0 = success, non-zero = error)
    int rc = ae_compute_chart_core(&in, &chart, err, sizeof(err));
    if (rc != 0) {
        printf("Chart computation failed: %s\n", err);
        return 1;
    }

    // Serialize
    char json[32768];
    int written = ae_chartcore_to_json(&chart, json, sizeof(json));
    if (written < 0) {
        printf("Serializer failed: buffer too small\n");
        return 1;
    }

    // Print JSON
    printf("=== SERIALIZER OUTPUT ===\n%s\n", json);

    // Print payload size
    printf("\nPayload size: %d bytes\n", written);

    return 0;
}
