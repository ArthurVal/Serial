message(STATUS "-- Build tests have been enabled")

# GTest AUTO-DL ###############################################################
if(${PROJECT_NAME}_TESTING_DL_GTEST)
  message(STATUS "Automatic GTest download requested: ...")

  # This is use to force CMake to build gtest with std=c++17, I don't know how
  # to use target_compile_feature() with FetchContent
  set(CMAKE_CXX_STANDARD 17)

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
    GIT_PROGRESS ON)

  FetchContent_GetProperties(googletest)

  if(NOT googletest_POPULATED)
    message(STATUS "Populating googletest:...")
    FetchContent_Populate(googletest)
    message(STATUS "Populating googletest: OK")
  endif()

  message(STATUS "Automatic GTest download request: OK")
  message(STATUS "GTest Source dir: ${googletest_SOURCE_DIR}")
  message(STATUS "GTest Bin dir: ${googletest_BINARY_DIR}")

  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)

  include(GoogleTest)
endif()

add_subdirectory(${PROJECT_NAME})
