PROJECT(vcd2qucsdat CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
cmake_policy(VERSION 3.10)

CONFIGURE_FILE (
    "${PROJECT_SOURCE_DIR}/../config.h.cmake"
    "${PROJECT_BINARY_DIR}/config.h"
)

add_compile_definitions(HAVE_CONFIG_H)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive- /MP /Zc:preprocessor)
else()
    # additional warnings
    add_compile_options(-Wall -Wextra)
endif()

#
# Need Flex
#
FIND_PACKAGE(FLEX 2.5.9  REQUIRED)
IF(FLEX_FOUND)
  #MESSAGE(STATUS "Found flex: ${FLEX_EXECUTABLE} / Version: ${FLEX_VERSION}" )
ENDIF()

#
# Need Bison
#
# This is a HACK to get arround a PATH issue with Qt Creator on OSX.
# It seams impossible to pass a custom PATH to Qt Creator on OSX, ie, cannot prepend `/usr/local/bin/` for intance.
# The FIND_PACKAGE fails. For now we provide a fallback with a custom FIND_PROGRAM. The variable BISON_DIR is also available.
FIND_PACKAGE(BISON 2.4 REQUIRED)
IF(BISON_FOUND)
  #MESSAGE(STATUS "Found bison: ${BISON_EXECUTABLE} / Version: ${BISON_VERSION}" )
ENDIF()


INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)


SET(QUCSCONV_SRC
  #check_spice.cpp
  check_vcd.cpp
  #matlab_producer.cpp
  #csv_producer.cpp
  qucs_producer.cpp
  qucsconv.cpp
  #touchstone_producer.cpp
  #check_citi.cpp
  #check_mdl.cpp
  dataset.cpp
  #matrix.cpp
  property.cpp
  real.cpp
  vector.cpp
  #check_csv.cpp
  #check_zvr.cpp
  #  hash.cpp  
  #matvec.cpp
  strlist.cpp
  check_dataset.cpp
  logging.c
  #check_touchstone.cpp
  #object.cpp
  #sweep.cpp
  complex.cpp
  #variable.cpp
  #substrate.cpp
  #equation.cpp
  #analysis.cpp
  #evaluate.cpp
  #differentiate.cpp
  #range.cpp
)

SET(ParserTypes
  #mdl
  #spice
  dataset
  vcd
)

# generated files are named like:
#   - parse_spice.cpp scan_spice.cpp tokens_spice.h

# clear list of generated files
SET(conv_generated)
FOREACH( type ${ParserTypes} )

  # Create custom Bison
  SET(BO_${type}
    ${CMAKE_CURRENT_BINARY_DIR}/parse_${type}.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/tokens_${type}.h )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${BO_${type}}
    COMMAND ${BISON_EXECUTABLE}
              --defines=${CMAKE_CURRENT_BINARY_DIR}/parse_${type}.hpp
              --output=${CMAKE_CURRENT_BINARY_DIR/}parse_${type}.cpp
              ${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/parse_${type}.ypp
    )

  # Create custom Flex
  SET(FI_${type} ${CMAKE_CURRENT_SOURCE_DIR}/scan_${type}.lpp )
  SET(FO_${type} ${CMAKE_CURRENT_BINARY_DIR}/scan_${type}.cpp )
  ADD_CUSTOM_COMMAND(
    OUTPUT ${FO_${type}}
    COMMAND ${FLEX_EXECUTABLE}
            --outfile=${FO_${type}}
            ${FI_${type}}
    DEPENDS ${BO_${type}} ${FI_${type}}
    )
  SET(conv_generated ${conv_generated} ${FO_${type}})
  SET(conv_generated ${conv_generated} ${BO_${type}})
ENDFOREACH()
FOREACH( gfile ${conv_generated} )
  SET_SOURCE_FILES_PROPERTIES(${gfile} PROPERTIES GENERATED TRUE)
ENDFOREACH()

ADD_EXECUTABLE(${PROJECT_NAME} ${QUCSCONV_SRC} ${conv_generated} )

#
# Handle installation
#
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)
