include_directories(
  ${qucs-core_SOURCE_DIR} ${qucs-core_CURRENT_SOURCE_DIR}
  ${qucs-core_SOURCE_DIR}/src/math ${qucs-core_SOURCE_DIR}/src/components
)# component.h

set(DEVICES_SRC
    bjt.cpp
    device.cpp
    diac.cpp
    diode.cpp
    eqndefined.cpp
    jfet.cpp
    mosfet.cpp
    thyristor.cpp
    triac.cpp
    tunneldiode.cpp)

add_library(coreDevices OBJECT ${DEVICES_SRC})

set(HEADERS device.h)

install(FILES ${HEADERS} DESTINATION include/qucs-core)
