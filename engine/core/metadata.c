#include "metadata.h"
#include "swephexp.h"
#include <stdio.h>
#include <string.h>

// -----------------------------------------
// Safe error setter
// -----------------------------------------
static void ae_set_error(char *err_msg, int err_msg_size, const char *msg) {
    if (err_msg && err_msg_size > 0) {
        strncpy(err_msg, msg, err_msg_size - 1);
        err_msg[err_msg_size - 1] = '\0';
    }
}

// -----------------------------------------
// Compute UT hour from local time + timezone
// -----------------------------------------
static double ae_compute_ut_hour(const AeInput *in) {
    double h = (double)in->hour +
               (double)in->minute / 60.0 +
               (double)in->second / 3600.0;

    // UT = local time - timezone offset
    return h - in->timezone;
}

// -----------------------------------------
// Main metadata computation
// -----------------------------------------
int ae_compute_metadata(
    const AeInput *in,
    double *jd_ut,
    double *lst_deg,
    char *err_msg,
    int err_msg_size
) {
    if (!in || !jd_ut || !lst_deg) {
        ae_set_error(err_msg, err_msg_size, "NULL pointer in metadata");
        return -1;
    }

    // 1) Compute UT hour
    double ut_hour = ae_compute_ut_hour(in);

    // 2) Compute Julian Day UT
    *jd_ut = swe_julday(
        in->year,
        in->month,
        in->day,
        ut_hour,
        SE_GREG_CAL
    );

    double gst_hours = swe_sidtime(*jd_ut);
    double gst_deg = gst_hours * 15.0;

    // Local Sidereal Time = GST + longitude
    double lst = gst_deg + in->lon;

    // Normalize to 0–360
    while (lst < 0) lst += 360.0;
    while (lst >= 360.0) lst -= 360.0;

    *lst_deg = lst;


    return 0;
}
