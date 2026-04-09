#include "swiss_init.h"
#include "swephexp.h"
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

void ae_init_swiss(const char *ephe_path) {
    printf("Using ephe path: %s\n", ephe_path);
    swe_set_ephe_path(ephe_path);
    swe_set_tid_acc(0);
}

void ae_init_swiss_relative() {
    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path)-1);
    if (len == -1) {
        swe_set_ephe_path("./external/swisseph/ephe");
        return;
    }

    exe_path[len] = '\0';
    char *dir = dirname(exe_path);

    char ephe_path[1024];
    snprintf(ephe_path, sizeof(ephe_path),
             "%s/../external/swisseph/ephe", dir);

    printf("Using relative ephe path: %s\n", ephe_path);
    swe_set_ephe_path(ephe_path);
    swe_set_tid_acc(0);
}
