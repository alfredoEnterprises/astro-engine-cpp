#include "engine.h"
#include "swephexp.h"
#include <string.h>

void ae_init_swiss(const char *ephe_path) {
    printf("Using ephe path: %s\n", ephe_path);
     swe_set_ephe_path(ephe_path);
     
     swe_set_tid_acc(0);
}



int ae_calc_sun(double jd_ut, AeBodySample *out) {
    printf("DEBUG: entering ae_calc_sun, jd_ut=%f\n", jd_ut);

    int32 iflag = SEFLG_SWIEPH | SEFLG_SPEED;
    double xx[6];
    char serr[256] = {0};

    printf("DEBUG: before swe_calc_ut\n");
    int32 ret = swe_calc_ut(jd_ut, SE_SUN, iflag, xx, serr);
    printf("DEBUG: after swe_calc_ut, ret=%d, serr=%s\n", ret, serr);
    if (ret < 0) {
        out->serr_code = ret;
        return ret;
    }

    out->jd_ut      = jd_ut;
    out->lon        = xx[0];
    out->lat        = xx[1];
    out->dist       = xx[2];
    out->speed_lon  = xx[3];
    out->iflag      = ret;
    out->serr_code  = 0;
    return 0;
}
