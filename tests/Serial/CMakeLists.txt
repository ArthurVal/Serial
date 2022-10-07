project(test_${PROJECT_NAME})

add_executable(${PROJECT_NAME}
  test_serializer.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
#  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE gtest_main
  PRIVATE gmock_main
  PRIVATE serial::serial
)

target_compile_options(${PROJECT_NAME}
  PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic
)

gtest_discover_tests(${PROJECT_NAME})

add_executable(serial::test ALIAS ${PROJECT_NAME})
add_dependencies(tests serial::test)
