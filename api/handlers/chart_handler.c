#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

// Extract the real JSON payload from raw event JSON
static char* normalize_event_json(const char* raw) {
    // Check if the raw JSON contains "body"
    const char *body_key = "\"body\":";

    char *found = strstr(raw, body_key);
    if (!found) {
        // No "body" → raw JSON is the payload
        return strdup(raw);
    }

    // Move pointer to after "body":
    found += strlen(body_key);

    // Skip whitespace
    while (*found == ' ' || *found == '\n' || *found == '\t') found++;

    // Case 1: body is a STRING → starts with "
    if (*found == '\"') {
        found++; // skip opening quote

        // Extract until closing quote
        const char *end = strchr(found, '\"');
        if (!end) return strdup(raw);

        size_t len = end - found;
        char *inner = malloc(len + 1);
        memcpy(inner, found, len);
        inner[len] = '\0';
        return inner;
    }

    // Case 2: body is an OBJECT → starts with {
    if (*found == '{') {
        // Find matching closing brace
        const char *end = strrchr(found, '}');
        if (!end) return strdup(raw);

        size_t len = end - found + 1;
        char *inner = malloc(len + 1);
        memcpy(inner, found, len);
        inner[len] = '\0';
        return inner;
    }

    // Fallback: return raw
    return strdup(raw);
}

int main() {
    // 1. Read request JSON
    char *raw_json = read_stdin();
    if (!raw_json) {
        printf("{\"ok\":false,\"error\":\"Failed to read input\"}");
        return 0;
    }

    // 2. Normalize event → extract real payload
    char *input_json = normalize_event_json(raw_json);
    free(raw_json);

    // 3. Validate → AeInput
    AeInput input;
    char err[256];
    memset(err, 0, sizeof(err));

    bool ok = ae_validate_chart_request(input_json, &input, err, sizeof(err));
    free(input_json);

    if (!ok) {
        printf("{\"ok\":false,\"error\":\"%s\"}", err);
        return 0;
    }

    // 4. Compute chart
    AeChartCore core;
    memset(&core, 0, sizeof(core));

    char engine_err[256];
    memset(engine_err, 0, sizeof(engine_err));

    int rc = ae_compute_chart_core(&input, &core, engine_err, sizeof(engine_err));
    if (rc != 0) {
        printf("{\"ok\":false,\"error\":\"%s\"}", engine_err);
        return 0;
    }

    // 5. Serialize chart
    char json_chart[65536];
    int written = ae_chartcore_to_json(&core, json_chart, sizeof(json_chart));

    if (written < 0) {
        printf("{\"ok\":false,\"error\":\"Serialization error\"}");
        return 0;
    }

    // 6. Output final response
    printf("{\"ok\":true,\"chart\":%s}", json_chart);
    return 0;
}
