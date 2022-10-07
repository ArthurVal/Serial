#.rst:
# SerialOptions
# -------------------
#
# This module is meant to be included by the main CMake of the Serial project.
#
# It declares the following options, prefixed by Serial:
#   _ENABLE_TESTING     - BOOL - Build unittests.
#     _TESTING_DL_GTEST - BOOL - If TESTING is enable, DL GTest by hand.
#   _ENABLE_DOC         - BOOL - Build the doc.

include(CMakeDependentOption)   #for cmake_dependent_option
include(CMakePrintHelpers)      #for cmake_print_variables & cmake_print_properties

message(STATUS "${PROJECT_NAME} options...")

option(${PROJECT_NAME}_ENABLE_TESTING "Enable the unit tests build" OFF)
cmake_print_variables(${PROJECT_NAME}_ENABLE_TESTING)

cmake_dependent_option(${PROJECT_NAME}_TESTING_DL_GTEST
  "Enable the auto download of googletest lib" ON ${PROJECT_NAME}_ENABLE_TESTING
  OFF)
cmake_print_variables(${PROJECT_NAME}_TESTING_DL_GTEST)

option(${PROJECT_NAME}_ENABLE_DOC "Build the doxygen documentation" OFF)
cmake_print_variables(${PROJECT_NAME}_ENABLE_DOC)
