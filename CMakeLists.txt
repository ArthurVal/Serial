file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
  message(FATAL_ERROR "You cannot build in a source directory (or any"
    " directory with CMakeLists.txt file). Please make a build subdirectory. "
    "Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

cmake_minimum_required(VERSION 3.11)

project(Serial
  VERSION 0.0.1
  DESCRIPTION "Library used to do serialization/de-serialization things"
  LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Options.cmake)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_file(
  include/${PROJECT_NAME}/Config.hpp.in
  include/${PROJECT_NAME}/Config.hpp
  @ONLY
)

if(NOT APPLE)
  set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

add_subdirectory(src)

if(${PROJECT_NAME}_ENABLE_TESTING)
  enable_testing()
  add_custom_target(tests)
  add_subdirectory(tests)
endif()

# if(${PROJECT_NAME}_ENABLE_DOC)
#   add_subdirectory(docs)
# endif()
