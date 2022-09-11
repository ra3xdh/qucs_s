include_directories(
  ${qucs-core_SOURCE_DIR} ${qucs-core_CURRENT_SOURCE_DIR}
  ${qucs-core_SOURCE_DIR}/src/math ${qucs-core_SOURCE_DIR}/src/components
)# component.h

set(DIGITAL_SRC
    and.cpp
    buffer.cpp
    digisource.cpp
    digital.cpp
    inverter.cpp
    nand.cpp
    nor.cpp
    or.cpp
    xnor.cpp
    xor.cpp)

add_library(coreDigital OBJECT ${DIGITAL_SRC})
