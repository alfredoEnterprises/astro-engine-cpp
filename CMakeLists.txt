cmake_minimum_required(VERSION 3.15)
project(astro_engine_cpp C CXX)

set(CMAKE_C_STANDARD 99)

# ----------------------------------------
# Swiss Ephemeris static library
# ----------------------------------------
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

# Make headers visible BEFORE compiling sources
include_directories(engine)
include_directories(external/swisseph)

add_library(astro_engine STATIC
    engine/engine.c
    engine/data_types.c
    engine/metadata.c
    engine/bodies.c
    engine/houses.c
)

target_include_directories(astro_engine PUBLIC engine external/swisseph)

target_link_libraries(astro_engine PUBLIC swisseph m)

# ----------------------------------------
# Test executable
# ----------------------------------------
add_executable(test_engine
    tests/test_engine.cpp
)

target_include_directories(test_engine PUBLIC engine external/swisseph)

target_link_libraries(test_engine
    astro_engine
    swisseph
    m
)
