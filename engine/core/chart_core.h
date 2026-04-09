#ifndef AE_CHART_CORE_H
#define AE_CHART_CORE_H

#include "data_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int ae_compute_chart_core(const AeInput *in,
                          AeChartCore *out,
                          char *err,
                          size_t err_size);

#ifdef __cplusplus
}
#endif

#endif
