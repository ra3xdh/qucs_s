include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(MATH_SRC # cbesselj.cpp
    complex.cpp fspecial.cpp matrix.cpp real.cpp)

set(HEADERS complex.h matrix.h precision.h real.h)

add_library(coreMath OBJECT ${MATH_SRC})

install(FILES ${HEADERS} DESTINATION include/qucs-core)
