#ifndef AE_ENGINE_H
#define AE_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double jd_ut;
    double lon;
    double lat;
    double dist;
    double speed_lon;
    int    iflag;
    int    serr_code;
} AeBodySample;

void ae_init_swiss(const char *ephe_path);
int  ae_calc_sun(double jd_ut, AeBodySample *out);

#ifdef __cplusplus
}
#endif

#endif
