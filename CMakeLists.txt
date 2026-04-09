cmake_minimum_required(VERSION 3.10)
project(astro_engine C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# ---------------------------------------------------------
# Swiss Ephemeris (build local static library)
# ---------------------------------------------------------
# ---------------------------------------------------------
# Swiss Ephemeris (static library)
# ---------------------------------------------------------
add_library(swisseph STATIC
    external/swisseph/swedate.c
    external/swisseph/swehouse.c
    external/swisseph/swejpl.c
    external/swisseph/swemmoon.c
    external/swisseph/swemplan.c
    external/swisseph/sweph.c
    external/swisseph/swephlib.c
    external/swisseph/swecl.c
    external/swisseph/swehel.c
)

target_include_directories(swisseph PUBLIC external/swisseph)


# ---------------------------------------------------------
# Engine core
# ---------------------------------------------------------
add_library(astro_engine_core
    engine/core/data_types.c
    engine/core/metadata.c
    engine/core/bodies.c
    engine/core/houses.c
    engine/core/aspects.c
    engine/core/points.c
    engine/core/aspect_rules.c
    engine/core/chart_core.c
)

target_include_directories(astro_engine_core PUBLIC
    engine/core
    external/swisseph        # ← REQUIRED FIX
)

target_link_libraries(astro_engine_core
    swisseph
    m
)

# ---------------------------------------------------------
# Bootstrap (Swiss Ephemeris init)
# ---------------------------------------------------------
add_library(astro_engine_bootstrap
    engine/bootstrap/swiss_init.c
)

target_include_directories(astro_engine_bootstrap PUBLIC
    engine/bootstrap
    external/swisseph        # ← REQUIRED FIX
)

target_link_libraries(astro_engine_bootstrap
    swisseph
    m
)

# ---------------------------------------------------------
# API serializers
# ---------------------------------------------------------
add_library(astro_api_serializers
    api/serializers/chart_json.c
)

target_include_directories(astro_api_serializers PUBLIC
    api/serializers
    engine/core
)

target_link_libraries(astro_api_serializers
    astro_engine_core
)

# ---------------------------------------------------------
# Tests
# ---------------------------------------------------------
add_executable(test_core
    tests/test_core.cpp
)

target_include_directories(test_core PUBLIC
    engine/core
    engine/bootstrap
    api/serializers
    external/swisseph        # ← OPTIONAL but helpful
)

target_link_libraries(test_core
    astro_engine_core
    astro_engine_bootstrap
    astro_api_serializers
    swisseph
    m
)

add_executable(test_serializers
    tests/test_serializers.cpp
)

target_include_directories(test_serializers PUBLIC
    engine/core
    api/serializers
)

target_link_libraries(test_serializers
    astro_engine_core
    astro_api_serializers
    swisseph
    m
)

