project(qucs-s CXX)
cmake_minimum_required(VERSION 3.10)
cmake_policy(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

add_library(geometry STATIC
    "concepts.h"
    "geometry.h"
    "multi_point.h"
    "one_point.h"
    "shapes.h"
    )

add_executable(test_geometry test_geometry.cpp)
add_test(NAME GeometryTest COMMAND test_geometry)