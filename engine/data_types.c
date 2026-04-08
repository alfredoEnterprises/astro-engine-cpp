#include "data_types.h"
#include <string.h>

void ae_init_chart_core(AeChartCore *c) {
    memset(c, 0, sizeof(AeChartCore));
}

void ae_init_input(AeInput *in) {
    memset(in, 0, sizeof(AeInput));
    in->house_system = 'P'; // default Placidus
}
