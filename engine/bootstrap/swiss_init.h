#ifndef AE_SWISS_INIT_H
#define AE_SWISS_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Initialize Swiss Ephemeris using a relative or absolute path.
void ae_init_swiss(const char *ephe_path);

// Initialize Swiss Ephemeris using a path relative to the binary location.
void ae_init_swiss_relative();

#ifdef __cplusplus
}
#endif

#endif
