#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../validation/validation.h"
#include "../../engine/core/chart_core.h"
#include "../serializers/chart_json.h"

// Read entire STDIN into a buffer
static char* read_stdin() {
    size_t size = 0;
    size_t cap = 4096;
    char *buf = malloc(cap);
    if (!buf) return NULL;

    int c;
    while ((c = fgetc(stdin)) != EOF) {
        if (size + 1 >= cap) {
            cap *= 2;
            char *newbuf = realloc(buf, cap);
            if (!newbuf) {
                free(buf);
                return NULL;
            }
            buf = newbuf;
        }
        buf[size++] = (char)c;
    }
    buf[size] = '\0';
    return buf;
}

int main() {
    // 1. Read request JSON
    char *input_json = read_stdin();
    if (!input_json) {
        printf("{\"ok\":false,\"error\":\"Failed to read input\"}");
        return 0;
    }

    // 2. Validate → AeInput
    AeInput input;
    char err[256];
    memset(err, 0, sizeof(err));

    bool ok = ae_validate_chart_request(input_json, &input, err, sizeof(err));
    free(input_json);

    if (!ok) {
        printf("{\"ok\":false,\"error\":\"%s\"}", err);
        return 0;
    }


    // 3. Compute chart
AeChartCore core;
memset(&core, 0, sizeof(core));

char engine_err[256];
memset(engine_err, 0, sizeof(engine_err));

int rc = ae_compute_chart_core(&input, &core, engine_err, sizeof(engine_err));
if (rc != 0) {
    printf("{\"ok\":false,\"error\":\"%s\"}", engine_err);
    return 0;
}


    // 4. Serialize chart
    char json_chart[65536];
    int written = ae_chartcore_to_json(&core, json_chart, sizeof(json_chart));

    if (written < 0) {
        printf("{\"ok\":false,\"error\":\"Serialization error\"}");
        return 0;
    }

    // 5. Output final response
    printf("{\"ok\":true,\"chart\":%s}", json_chart);
    return 0;
}
