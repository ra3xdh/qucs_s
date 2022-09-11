cmake_minimum_required(VERSION 3.0)


project(qucs-doc NONE)

# Append -shell-escape to defaults
# pdflatex may use the shell to run gnuplot (report/activefilter/)
set(LATEX_COMPILER_FLAGS
    "-interaction=batchmode -file-line-error -shell-escape"
    CACHE STRING "Flags passed to latex.")

include( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/UseLATEX.cmake )

# check for fig2dev
find_program(FIG2DEV fig2dev)
if(FIG2DEV)
    message(STATUS "Using ${FIG2DEV} to convert figures.")
else()
    message(FATAL_ERROR "fig2dev was requested but not found.")
endif()

find_program(PYTHON python3)
if(PYTHON)
    message(STATUS "Using ${PYTHON} to generate figures.")
else()
    message(FATAL_ERROR "python3 was requested but not found.")
endif()

add_subdirectory(report)
add_subdirectory(technical)
add_subdirectory(tutorial)


# Custom uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake" IMMEDIATE @ONLY)

add_custom_target(
  uninstall-doc COMMAND ${CMAKE_COMMAND} -P
                  ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
