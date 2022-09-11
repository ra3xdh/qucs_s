project(qucsator CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#
# Checks for libraries.
#
#AC_CHECK_LIB(m, sin) need to check for sin?
IF(NOT WIN32)
  FIND_LIBRARY(MATH_LIB NAMES m)
  IF(NOT MATH_LIB)
    MESSAGE(SEND_ERROR "Math lib not found: ${MATH_LIB}" )
  ELSE()
    MESSAGE(STATUS "Math lib found at: ${MATH_LIB}" )
  ENDIF()
ENDIF()

#
# Checks for header files.
# AC_HEADER_STDC !! obsolete, need to check?
# Define STDC_HEADERS if the system has ANSI C header files.
# Specifically, this macro checks for `stdlib.h', `stdarg.h', `string.h', and `float.h';
# Lifted the cmake checks from gd-libdg, Lua
# https://bitbucket.org/libgd/gd-libgd
# https://github.com/LuaDist/libgd/tree/master/cmake/modules
INCLUDE (CheckIncludeFiles)
SET(CMAKE_REQUIRED_INCLUDES "/usr/include" "/usr/local/include")
SET(CMAKE_MODULE_PATH "${qucs-core_SOURCE_DIR}/cmake/modules")
INCLUDE(AC_HEADER_STDC)

#
# Further header checks
#
INCLUDE (CheckIncludeFile)

# list of headers to be checked
SET( INCLUDES
  ieeefp.h
  memory.h
  stddef.h
  stdlib.h
  string.h
  unistd.h
)

#
# Check if header can be included.
#  * Define HAVE_[basename]_H to 1 if you have the header.
#
FOREACH( header ${INCLUDES} )
  GET_FILENAME_COMPONENT(base ${header} NAME_WE)
  STRING(TOUPPER ${base} base)
  CHECK_INCLUDE_FILE( ${header} HAVE_${base}_H )
  #MESSAGE(STATUS "${header}  --> ${HAVE_${base}_H}")
ENDFOREACH()

# Checks for typedefs, structures, and compiler characteristics.
# AC_C_CONST  !!obsolete
# AC_C_CONST "This macro is obsolescent, as current C compilers support `const'.
# New programs need not use this macro."

#
# Check for type sizes.
#
INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE("short" SIZEOF_SHORT)
CHECK_TYPE_SIZE("int"   SIZEOF_INT)
CHECK_TYPE_SIZE("long"  SIZEOF_LONG)
CHECK_TYPE_SIZE("double"  SIZEOF_DOUBLE)
CHECK_TYPE_SIZE("long double"  SIZEOF_LONG_DOUBLE)
#MESSAGE(STATUS "short  ${SIZEOF_SHORT}" )
#MESSAGE(STATUS "int   ${SIZEOF_INT}" )
#MESSAGE(STATUS "long  ${SIZEOF_LONG}" )
#MESSAGE(STATUS "double ${SIZEOF_DOUBLE}" )
#MESSAGE(STATUS "long double ${SIZEOF_LONG_DOUBLE}" )

#
# Check for double type.
#  * valid types are: double, float and long double.
#  * defines: nr_double_t,  The global type of double representation.
#  * defines: NR_DOUBLE_SIZE,  The size of the double representation.
#  * Use -DENABLE-DOUBLE="[float,double, long double]"
IF( ENABLE-DOUBLE )
  # User defined
  SET(DoubleType ${ENABLE-DOUBLE})

  # valid types
  SET(ValidTypes "float" "double" "long double" )

  LIST(FIND ValidTypes ${DoubleType} HasType)
  IF ( HasType EQUAL -1)
    MESSAGE(FATAL_ERROR "Valid types are: ${ValidTypes}")
  ENDIF()

  # The global type of double representation.
  SET( nr_double_t  DoubleType )
  CHECK_TYPE_SIZE(${DoubleType}  DoubleSize)

  # The size of the double representation.
  SET( NR_DOUBLE_SIZE ${DoubleSize} )
ELSE()
  # Default double
  SET(DoubleType "double")
  # The global type of double representation.
  SET( nr_double_t ${DoubleType} )
  CHECK_TYPE_SIZE(${DoubleType}  DoubleSize)

  # The size of the double representation.
  SET( NR_DOUBLE_SIZE ${DoubleSize} )
ENDIF()
MESSAGE(STATUS "using double type: ${DoubleType}; size: ${DoubleSize}")

# defines used in qucs_typedefs.h
SET(QUCS_INT32_TYPE ${nr_int32_t})
SET(QUCS_INT16_TYPE ${nr_int16_t})
SET(QUCS_DOUBLE_TYPE ${DoubleType})
SET(QUCS_DOUBLE_SIZE ${DoubleSize})
#
# Configure the header qucs_typedefs.h, interpolate above definitions.
#
CONFIGURE_FILE (
    "${qucs-core_SOURCE_DIR}/qucs_typedefs.h.cmake"
    "${qucs-core_BINARY_DIR}/qucs_typedefs.h"
)

#
# Check for library functions
#  * not all functons seem to be used after defined. TODO check for HAVE_{func}
#
INCLUDE(CheckFunctionExists)
SET(REQUIRED_FUNCTIONS
  floor pow exp sqrt log10 log
  cos sin
  acos asin                         # for real.cpp
  tan atan sinh
	cosh tanh fabs modf atan2 jn yn
  erf erfc                          # for fspecial.cpp
  round trunc acosh asinh           # for real.cpp
  strdup strerror
  strchr )                          # for compat.h, matvec.cpp, scan_*.cpp

FOREACH( func ${REQUIRED_FUNCTIONS} )
  STRING(TOUPPER ${func} FNAME)
  CHECK_FUNCTION_EXISTS(${func} HAVE_${FNAME})
  #message(STATUS "${func}  --> ${HAVE_${FNAME}}")
ENDFOREACH()

#
# Checks for complex classes and functions, as in the Autotools scripts.
#
# AC_CXX_NAMESPACES !!custom m4
# AC_CXX_HAVE_COMPLEX !!custom m4
# AC_CXX_HAVE_TR1_COMPLEX !!custom m4
# AC_CHECK_CXX_COMPLEX_FUNCS([cos cosh exp log log10 sin sinh sqrt tan tanh]) !!custom m4
# AC_CHECK_CXX_COMPLEX_FUNCS([acos acosh asin asinh atan atanh])
# AC_CHECK_CXX_COMPLEX_FUNCS([log2 norm])
# AC_CHECK_CXX_COMPLEX_POW
# AC_CHECK_CXX_COMPLEX_ATAN2         !failed, need libstdc?
# AC_CHECK_CXX_COMPLEX_FMOD          !failed:
# AC_CHECK_CXX_COMPLEX_POLAR
# AC_CHECK_CXX_COMPLEX_POLAR_COMPLEX !failed

#
# Namespace
#
# Check whether the compiler implements namespaces
#
TRY_COMPILE( HAVE_NAMESPACES
        ${CMAKE_BINARY_DIR}
        ${qucs-core_SOURCE_DIR}/cmake/namespaces.cpp
        OUTPUT_VARIABLE TRY_OUT)
IF(NOT HAVE_NAMESPACES)
  MESSAGE(SEND_ERROR "${PROJECT_NAME} requires an c++ compiler with namespace HAVE_NAMESPACES failed")# ${TRY_OUT}")
ENDIF()

#
# Check whether the compiler has complex<T>
#
TRY_COMPILE( HAVE_COMPLEX
        ${CMAKE_BINARY_DIR}
        ${qucs-core_SOURCE_DIR}/cmake/complex.cpp
        OUTPUT_VARIABLE TRY_OUT)
IF(NOT HAVE_COMPLEX)
  MESSAGE(SEND_ERROR "HAVE_COMPLEX failed")# ${TRY_OUT}")
ENDIF()


#
# Check std::vector::erase iterator type [const_iterator | iterator]
#
MESSAGE(STATUS "Checking HAVE_ERASE_CONSTANT_ITERATOR")
TRY_COMPILE( HAVE_ERASE_CONSTANT_ITERATOR
        ${CMAKE_BINARY_DIR}
        ${qucs-core_SOURCE_DIR}/cmake/erase_iterator_type.cpp
        OUTPUT_VARIABLE TRY_OUT)
IF(HAVE_ERASE_CONSTANT_ITERATOR)
  MESSAGE(STATUS "Using std::vector:erase iterator type : const_iterator")
ELSE()
  MESSAGE(STATUS "Using std::vector:erase iterator type : iterator")
ENDIF()

#
# Check for list of complex functions.
#
SET(COMPLEX_FUNCS_GRP1 acos acosh asin asinh atan atanh cos cosh
                   exp log log10 sin sinh sqrt tan tanh log2 norm )
SET(COMPLEX_FUNCS_GRP2 pow atan2 fmod polar polar_complex )

#
# test complex function group 1
# code inlined to easily replace '${func}'
#
FOREACH(func ${COMPLEX_FUNCS_GRP1})
  SET(code
    " #include <complex>
      using namespace std\;
    #ifdef log2
    #undef log2
    #endif
    int main() {
      complex<double> a\;
      ${func}(a)\;
      return 0\;
    }")

  FILE(WRITE ${qucs-core_SOURCE_DIR}/cmake/test_${func}.cpp ${code})

  string(TOUPPER ${func} FNAME)

  message(STATUS "Checking HAVE_CXX_COMPLEX_${FNAME}")

  TRY_COMPILE( HAVE_CXX_COMPLEX_${FNAME}
          ${CMAKE_BINARY_DIR}
          ${qucs-core_SOURCE_DIR}/cmake/test_${func}.cpp
          OUTPUT_VARIABLE TRY_OUT)
  if(NOT HAVE_CXX_COMPLEX_${FNAME})
    message(STATUS "HAVE_CXX_COMPLEX_${FNAME} failed")# ${TRY_OUT}")
  ENDIF()

  FILE(REMOVE ${qucs-core_SOURCE_DIR}/cmake/test_${func}.cpp ${code})
ENDFOREACH()

#
# test complex function group 2
# use prepared source file. 
#
FOREACH(func ${COMPLEX_FUNCS_GRP2})
  string(TOUPPER ${func} FNAME)

  message(STATUS "Checking HAVE_CXX_COMPLEX_${FNAME}")

  TRY_COMPILE( HAVE_CXX_COMPLEX_${FNAME}
          ${CMAKE_BINARY_DIR}
          ${qucs-core_SOURCE_DIR}/cmake/complex_${func}.cpp
          COMPILE_DEFINITIONS -DHAVE_NAMESPACES -DHAVE_COMPLEX -DHAVE_TR1_COMPLEX
          OUTPUT_VARIABLE TRY_OUT)
  if(NOT HAVE_CXX_COMPLEX_${FNAME})
    message(STATUS "HAVE_CXX_COMPLEX_${FNAME} failed")# ${TRY_OUT}")
  ENDIF()
ENDFOREACH()

#
# Configure the header config.h, interpolate above definitions.
#
CONFIGURE_FILE (
    "${qucs-core_SOURCE_DIR}/config.h.cmake"
    "${qucs-core_BINARY_DIR}/config.h"
)

#
# List of lexer/parsers type names
#
SET(ParserTypes
	csv
  citi
  dataset
  mdl
  netlist
  touchstone
  zvr
)

SET(generated_SRC)

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
  SET(generated_SRC ${generated_SRC} ${FO_${type}})
  SET(generated_SRC ${generated_SRC} ${BO_${type}})
ENDFOREACH()

FOREACH( gfile ${generated_SRC} )
  SET_SOURCE_FILES_PROPERTIES(${gfile} PROPERTIES GENERATED TRUE)
ENDFOREACH()

#
# Source code libqucs
#
SET(LIBQUCS_SRC
  ${generated_SRC}
  analysis.cpp
  check_zvr.cpp
  interpolator.cpp
  parasweep.cpp
  property.cpp
  range.cpp
  spline.cpp
  strlist.cpp
  trsolver.cpp
  acsolver.cpp
  check_citi.cpp
  check_csv.cpp
  check_dataset.cpp
  check_mdl.cpp
  check_netlist.cpp
  check_touchstone.cpp
  circuit.cpp
  dataset.cpp
  dcsolver.cpp
  devstates.cpp
  differentiate.cpp
  environment.cpp
  equation.cpp # <= depends on gperfapphash.cpp
  evaluate.cpp
  exception.cpp
  exceptionstack.cpp
  fourier.cpp
  hbsolver.cpp
  history.cpp
  input.cpp
  integrator.cpp
  logging.c
  matvec.cpp
  module.cpp
  net.cpp
  nodelist.cpp
  nodeset.cpp
  object.cpp
  receiver.cpp
  spsolver.cpp
  sweep.cpp
  transient.cpp
  variable.cpp
  vector.cpp
)


#
# Template classes
#
SET(TEMPLATES tmatrix.h tvector.h eqnsys.h nasolver.h states.h tvector.h
	            ptrlist.h tridiag.h hash.h valuelist.h nasolution.h )

#
# Include headers to be installed
#
SET(PUBLIC_HEADERS
  ${qucs-core_BINARY_DIR}/config.h
  ${qucs-core_BINARY_DIR}/qucs_typedefs.h
  circuit.h
  compat.h
  constants.h
  consts.h
  integrator.h
  logging.h
  net.h
  netdefs.h
  node.h
  object.h
  states.h
  valuelist.h
  vector.h
  property.h
  ptrlist.h
  characteristic.h
  pair.h
  operatingpoint.h
)

INCLUDE_DIRECTORIES( ${qucs-core_SOURCE_DIR}          # generated config.h
                     ${qucs-core_SOURCE_DIR}/src/math # precision.h
                     ${qucs-core_SOURCE_DIR}/src/     # compat.h
                     ${qucs-core_SOURCE_DIR}/src/components # microstrip/substrate.h
                     ${qucs-core_SOURCE_DIR}/src/interface
                     ${qucs-core_BINARY_DIR}          # cmake generated config.h
                     ${qucs-core_BINARY_DIR}/src      # cmake generated gperfapphash.h
                     ${qucs-core_BINARY_DIR}/src/components   # generated verilog/[].core.h
                     )


#
# Replace 'evaluate::[whatever]' by NULL
#
#	* evaluate.h (class evaluate): New class implementing the
#	actual evaluation function (applications) for the equations
#	in Qucs.
#
ADD_CUSTOM_COMMAND(
  OUTPUT gperfappgen.h
  COMMAND ${SED_TOOL} -e 's/evaluate::[a-zA-Z0-9_]*/NULL/g'
    < ${CMAKE_CURRENT_SOURCE_DIR}/applications.h
    > ${CMAKE_CURRENT_BINARY_DIR}/gperfappgen.h
  DEPENDS ${applications.h}
)

#
# Compile gperfappgen
# * used to generate gperf input file (used in qucsator)
#
SET(gperf_SRC gperfappgen.cpp  gperfappgen.h )

ADD_EXECUTABLE(gperfappgen ${gperf_SRC})

#
# Run gperfappgen, pipe to gperf input to gperfapphash.gph
#
ADD_CUSTOM_COMMAND(
  OUTPUT gperfapphash.gph
  COMMAND gperfappgen > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph
  DEPENDS ${gperfappgen}
)

#
# Run gperf, create hash table.
#  * -I, Include the necessary system include files at the beginning of the code.
#  * -m, Perform multiple iterations to minimize generated table.
#  * Replace '{""}' by '{"",0}; (why?)
#
ADD_CUSTOM_COMMAND(
    OUTPUT gperfapphash.cpp
    COMMAND ${GPERF_TOOL} -I -m 8 ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.gph > temp.gperf
    COMMAND ${SED_TOOL} -e 's/{""},/{"",0},/g' < temp.gperf > ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp
    DEPENDS gperfapphash.gph
)

#  target <- source (includea)
# equation.cpp: gperfapphash.cpp
#
# noinst_PROGRAMS = gperfappgen
# gperfappgen_SOURCES = gperfappgen.cpp

# for cleaning (autogenerated)
#set(gperf_FILES gperfapphash.cpp gperfapphash.gph gperfappgen.h)


# Qucs library dependencies
ADD_SUBDIRECTORY( interface )
ADD_SUBDIRECTORY( components )
ADD_SUBDIRECTORY( components/digital )
ADD_SUBDIRECTORY( components/devices )
ADD_SUBDIRECTORY( components/microstrip )
ADD_SUBDIRECTORY( components/verilog )
ADD_SUBDIRECTORY( math )

# Qucsconv application
ADD_SUBDIRECTORY( converter )

# Linux?
#set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--export-all-symbols")

#
# Create qucsator
#
ADD_EXECUTABLE( qucsator ucs.cpp ${PUBLIC_HEADERS} ${TEMPLATES})

#
# Build libqucs as SHARED, dynamic library
#
# After:
# - http://stackoverflow.com/questions/11429055/cmake-how-create-a-single-shared-library-from-all-static-libraries-of-subprojec
#
ADD_LIBRARY( libqucsator SHARED
  ${LIBQUCS_SRC}
  ${TEMPLATES}
  $<TARGET_OBJECTS:coreMath>
  $<TARGET_OBJECTS:coreComponents>
  $<TARGET_OBJECTS:coreInterface>
  $<TARGET_OBJECTS:coreVerilog>
  $<TARGET_OBJECTS:coreMicrostrip>
  $<TARGET_OBJECTS:coreDevices>
  $<TARGET_OBJECTS:coreDigital>
)

# rename the library to let it be libqucsator (not liblibqucsator)
SET_TARGET_PROPERTIES( libqucsator PROPERTIES OUTPUT_NAME qucsator )

#
# Create target to handle gperfapp dependency
#
ADD_CUSTOM_TARGET( equation DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gperfapphash.cpp equation.cpp )
ADD_DEPENDENCIES( libqucsator equation )

#
# Link qucsator and libqucsator
#
TARGET_LINK_LIBRARIES( qucsator libqucsator ${CMAKE_DL_LIBS})

#
# Handle install
#
INSTALL(TARGETS qucsator DESTINATION bin)

# set Windows runtime location for libqucsator
# See: http://www.cmake.org/pipermail/cmake/2010-June/037461.html
INSTALL(TARGETS libqucsator
     RUNTIME DESTINATION bin COMPONENT runtime
     ARCHIVE DESTINATION lib COMPONENT devel
     LIBRARY DESTINATION lib COMPONENT library)

INSTALL(FILES ${PUBLIC_HEADERS} DESTINATION include/qucs-core)
