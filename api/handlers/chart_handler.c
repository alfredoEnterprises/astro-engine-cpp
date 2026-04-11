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

// Unescape a JSON string literal (handles \" \\ \n \t)
static char* json_unescape(const char* src) {
    size_t len = strlen(src);
    char* out = malloc(len + 1);
    char* dst = out;

    for (size_t i = 0; i < len; i++) {
        if (src[i] == '\\' && i + 1 < len) {
            i++;
            switch (src[i]) {
                case 'n':  *dst++ = '\n'; break;
                case 't':  *dst++ = '\t'; break;
                case 'r':  *dst++ = '\r'; break;
                case '\\': *dst++ = '\\'; break;
                case '"':  *dst++ = '"';  break;
                default:   *dst++ = src[i]; break;
            }
        } else {
            *dst++ = src[i];
        }
    }

    *dst = '\0';
    return out;
}

// Extract the real JSON payload from raw event JSON
static char* normalize_event_json(const char* raw) {
    const char *body_key = "\"body\":";
    const char *pos = strstr(raw, body_key);

    if (!pos) {
        return strdup(raw);
    }

    pos += strlen(body_key);

    while (*pos == ' ' || *pos == '\n' || *pos == '\t') pos++;

    // Case 1: body is a STRING → extract full quoted string literal
    if (*pos == '\"') {
        pos++; // skip opening quote

        const char* start = pos;
        int escaped = 0;

        while (*pos) {
            if (!escaped && *pos == '\"') {
                break;
            }
            escaped = (!escaped && *pos == '\\');
            pos++;
        }

        if (*pos != '\"') {
            return strdup(raw);
        }

        size_t len = pos - start;
        char* raw_inner = malloc(len + 1);
        memcpy(raw_inner, start, len);
        raw_inner[len] = '\0';

        char* unescaped = json_unescape(raw_inner);
        free(raw_inner);
        return unescaped;
    }

    // Case 2: body is an OBJECT
    if (*pos == '{') {
        const char *end = strrchr(pos, '}');
        if (!end) return strdup(raw);

        size_t len = end - pos + 1;
        char *inner = malloc(len + 1);
        memcpy(inner, pos, len);
        inner[len] = '\0';
        return inner;
    }

    return strdup(raw);
}


int main() {
    // 1. Read request JSON
    char *raw_json = read_stdin();

   /* fprintf(stderr,
        "=== RAW STDIN RECEIVED BY LAMBDA ===\n%s\n=== END RAW STDIN ===\n",
        raw_json ? raw_json : "(null)"
    );*/

    if (!raw_json) {
        printf("{\"ok\":false,\"error\":\"Failed to read input\"}");
        return 0;
    }

    // 2. Normalize event → extract real payload
    char *input_json = normalize_event_json(raw_json);
    free(raw_json);
   
    fprintf(stderr,
        "\n\n###############################\n"
        "# BEGIN NORMALIZED JSON BLOCK #\n"
        "###############################\n"
        "%s\n"
        "#############################\n"
        "# END NORMALIZED JSON BLOCK #\n"
        "#############################\n\n",
        input_json
        );
        

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
