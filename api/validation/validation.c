#include "validation.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "../../engine/core/data_types.h"
#include "../../engine/core/chart_core.h"
#include "../third_party/cjson/cJSON.h"

// Helper: write error safely
static void set_err(char *err, size_t err_size, const char *msg) {
    if (err && err_size > 0) {
        snprintf(err, err_size, "%s", msg);
    }
}

// Helper: ensure a JSON field exists and is a number
static bool get_number(cJSON *obj, const char *key, double *out, char *err, size_t err_size) {
    cJSON *item = cJSON_GetObjectItem(obj, key);
    if (!item || !cJSON_IsNumber(item)) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Field '%s' is missing or not a number", key);
        set_err(err, err_size, buf);
        return false;
    }
    *out = item->valuedouble;
    return true;
}

// Helper: ensure a JSON field exists and is an integer
static bool get_int(cJSON *obj, const char *key, int *out, char *err, size_t err_size) {
    cJSON *item = cJSON_GetObjectItem(obj, key);
    if (!item || !cJSON_IsNumber(item)) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Field '%s' is missing or not an integer", key);
        set_err(err, err_size, buf);
        return false;
    }
    *out = item->valueint;
    return true;
}

// Helper: ensure a JSON field exists and is a string (for house system)
static bool get_string(cJSON *obj, const char *key, const char **out, char *err, size_t err_size) {
    cJSON *item = cJSON_GetObjectItem(obj, key);
    if (!item || !cJSON_IsString(item)) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Field '%s' is missing or not a string", key);
        set_err(err, err_size, buf);
        return false;
    }
    *out = item->valuestring;
    return true;
}

bool ae_validate_chart_request(
    const char *json,
    AeInput *out,
    char *err,
    size_t err_size
) {
    if (!json || !out) {
        set_err(err, err_size, "Invalid arguments");
        return false;
    }

    cJSON *root = cJSON_Parse(json);
    if (!root) {
        set_err(err, err_size, "Invalid JSON");
        return false;
    }

    memset(out, 0, sizeof(AeInput));

    // Extract fields
    if (!get_int(root, "year", &out->year, err, err_size)) goto fail;
    if (!get_int(root, "month", &out->month, err, err_size)) goto fail;
    if (!get_int(root, "day", &out->day, err, err_size)) goto fail;

    if (!get_int(root, "hour", &out->hour, err, err_size)) goto fail;
    if (!get_int(root, "minute", &out->minute, err, err_size)) goto fail;
    if (!get_int(root, "second", &out->second, err, err_size)) goto fail;

    if (!get_number(root, "timezone", &out->timezone, err, err_size)) goto fail;
    if (!get_number(root, "lat", &out->lat, err, err_size)) goto fail;
    if (!get_number(root, "lon", &out->lon, err, err_size)) goto fail;

    const char *hs = NULL;
    if (!get_string(root, "house_system", &hs, err, err_size)) goto fail;

    // Validate ranges
    if (out->year < 1800 || out->year > 2200) {
        set_err(err, err_size, "Year must be between 1800 and 2200");
        goto fail;
    }
    if (out->month < 1 || out->month > 12) {
        set_err(err, err_size, "Month must be 1–12");
        goto fail;
    }
    if (out->day < 1 || out->day > 31) {
        set_err(err, err_size, "Day must be 1–31");
        goto fail;
    }
    if (out->hour < 0 || out->hour > 23) {
        set_err(err, err_size, "Hour must be 0–23");
        goto fail;
    }
    if (out->minute < 0 || out->minute > 59) {
        set_err(err, err_size, "Minute must be 0–59");
        goto fail;
    }
    if (out->second < 0 || out->second > 59) {
        set_err(err, err_size, "Second must be 0–59");
        goto fail;
    }
    if (out->timezone < -12 || out->timezone > 14) {
        set_err(err, err_size, "Timezone must be between -12 and +14");
        goto fail;
    }
    if (out->lat < -90 || out->lat > 90) {
        set_err(err, err_size, "Latitude must be between -90 and 90");
        goto fail;
    }
    if (out->lon < -180 || out->lon > 180) {
        set_err(err, err_size, "Longitude must be between -180 and 180");
        goto fail;
    }

    // House system must be a single character
    if (strlen(hs) != 1) {
        set_err(err, err_size, "House system must be a single character (e.g., 'P')");
        goto fail;
    }

    out->house_system = hs[0];

    cJSON_Delete(root);
    return true;

fail:
    cJSON_Delete(root);
    return false;
}

bool ae_request_is_transit_window(const char *json) {
    if (!json) return false;
    cJSON *root = cJSON_Parse(json);
    if (!root) return false;
    cJSON *op = cJSON_GetObjectItem(root, "operation");
    bool is_tw = op && cJSON_IsString(op) &&
                 strcmp(op->valuestring, "transit-window") == 0;
    cJSON_Delete(root);
    return is_tw;
}

bool ae_validate_transit_window_request(
    const char *json,
    AeTransitRequest *out,
    char *err,
    size_t err_size
) {
    if (!json || !out) { set_err(err, err_size, "Invalid arguments"); return false; }

    cJSON *root = cJSON_Parse(json);
    if (!root) { set_err(err, err_size, "Invalid JSON"); return false; }

    memset(out, 0, sizeof(AeTransitRequest));
    out->hour = 12; // default noon UT

    cJSON *start = cJSON_GetObjectItem(root, "start");
    if (!start || !cJSON_IsObject(start)) {
        set_err(err, err_size, "Field 'start' missing or not an object");
        cJSON_Delete(root); return false;
    }

    if (!get_int(start, "year",  &out->year,  err, err_size)) { cJSON_Delete(root); return false; }
    if (!get_int(start, "month", &out->month, err, err_size)) { cJSON_Delete(root); return false; }
    if (!get_int(start, "day",   &out->day,   err, err_size)) { cJSON_Delete(root); return false; }

    // Optional time-of-day (defaults: hour=12, minute=0, second=0).
    cJSON *h = cJSON_GetObjectItem(start, "hour");
    if (h && cJSON_IsNumber(h)) out->hour = h->valueint;
    cJSON *mi = cJSON_GetObjectItem(start, "minute");
    if (mi && cJSON_IsNumber(mi)) out->minute = mi->valueint;
    cJSON *se = cJSON_GetObjectItem(start, "second");
    if (se && cJSON_IsNumber(se)) out->second = se->valueint;

    if (!get_int(root, "days", &out->days, err, err_size)) { cJSON_Delete(root); return false; }

    // Range checks (mirror chart_core.c domain validation + transit caps).
    if (out->year < 1600 || out->year > 2500) { set_err(err, err_size, "Year out of range (1600-2500)"); cJSON_Delete(root); return false; }
    if (out->month < 1 || out->month > 12)    { set_err(err, err_size, "Month out of range (1-12)");    cJSON_Delete(root); return false; }
    if (out->day < 1 || out->day > 31)        { set_err(err, err_size, "Day out of range (1-31)");      cJSON_Delete(root); return false; }
    if (out->hour < 0 || out->hour > 23)      { set_err(err, err_size, "Hour out of range (0-23)");     cJSON_Delete(root); return false; }
    if (out->minute < 0 || out->minute > 59)  { set_err(err, err_size, "Minute out of range (0-59)");   cJSON_Delete(root); return false; }
    if (out->second < 0 || out->second > 59)  { set_err(err, err_size, "Second out of range (0-59)");   cJSON_Delete(root); return false; }
    if (out->days < 1 || out->days > AE_TRANSIT_MAX_DAYS) { set_err(err, err_size, "days out of range (1-100)"); cJSON_Delete(root); return false; }

    cJSON_Delete(root);
    return true;
}
