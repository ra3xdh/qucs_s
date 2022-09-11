include_directories(
  ${qucs-core_SOURCE_DIR}
  ${qucs-core_CURRENT_SOURCE_DIR}
  ${qucs-core_SOURCE_DIR}/src/math
  ${qucs-core_SOURCE_DIR}/src/components # component.h
  ${qucs-core_SOURCE_DIR}/src/components/devices) # devices.h

set(VA_FILES
    andor4x2.va
    andor4x3.va
    andor4x4.va
    binarytogrey4bit.va
    comp_1bit.va
    comp_2bit.va
    comp_4bit.va
    dff_SR.va
    DLS_1ton.va
    DLS_nto1.va
    dmux2to4.va
    dmux3to8.va
    dmux4to16.va
    EKV26MOS.va
    fa1b.va
    fa2b.va
    gatedDlatch.va
    greytobinary4bit.va
    ha1b.va
    hpribin4bit.va
    jkff_SR.va
    log_amp.va
    logic_0.va
    logic_1.va
    MESFET.va
    mod_amp.va
    mux2to1.va
    mux4to1.va
    mux8to1.va
    nigbt.va
    pad2bit.va
    pad3bit.va
    pad4bit.va
    photodiode.va
    phototransistor.va
    potentiometer.va
    tff_SR.va
    vcresistor.va)

# XML scripts need to build, the order matters
set(XML_BUILD
   analogfunction.xml
   qucsVersion.xml
   qucsMODULEcore.xml
   qucsMODULEdefs.xml)

# Concatenate scripts into command: -e script1 [-e script2]
set(XML_CMD)
foreach(script ${XML_BUILD})
  set(XML_CMD ${XML_CMD} -e ${CMAKE_CURRENT_SOURCE_DIR}/${script})
endforeach()

# Process each Verilog-A file.
set(gen_SRC)
foreach(file ${VA_FILES})
  get_filename_component(_name ${file} NAME_WE)
  set(output
      ${_name}.${_name}.analogfunction.h
      ${_name}.${_name}.analogfunction.cpp
      ${_name}.${_name}.core.cpp
      ${_name}.core.h
      ${_name}.cpp
      ${_name}.defs.h)
  # custom command/rule to generate outputs with admsXml
  add_custom_command(
    OUTPUT ${output}
    COMMAND ${ADMSXML} ${CMAKE_CURRENT_SOURCE_DIR}/${file} ${XML_CMD} -o
            ${_name}
    DEPENDS ${file} ${XML_BUILD})
  list(APPEND gen_SRC ${_name}.cpp)
endforeach()

add_library(coreVerilog OBJECT ${gen_SRC})

# Distribute XML scripts
set(XML_DIST ${XML_BUILD} qucsMODULEgui.xml qucsMODULEguiJSONsymbol.xml)

set(MAKE_FILES cpp2lib.makefile va2cpp.makefile)

install(FILES ${XML_DIST} DESTINATION include/qucs-core)
install(FILES ${MAKE_FILES} DESTINATION include/qucs-core)
