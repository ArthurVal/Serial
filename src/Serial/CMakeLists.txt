###############################################################################
#                                   libSerial                                 #
###############################################################################
add_library(serial INTERFACE)

# target_link_libraries(serial
#   PUBLIC fmt::fmt
# )

target_include_directories(serial
  INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(serial INTERFACE cxx_std_17)
# target_compile_options(serial
#   PRIVATE -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic
# )

set_target_properties(serial
  PROPERTIES
  # OUTPUT_NAME ${PROJECT_NAME}
  # SOVERSION ${PROJECT_VERSION_MAJOR}
  # VERSION ${PROJECT_VERSION}
  INTERFACE_serial_MAJOR_VERSION ${PROJECT_VERSION_MAJOR}
  COMPATIBLE_INTERFACE_STRING ${PROJECT_VERSION_MAJOR}
  # DEBUG_POSTFIX Debug
)

add_library(serial::serial ALIAS serial)

install(TARGETS serial
  EXPORT SerialMainTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          COMPONENT ${PROJECT_NAME}_Runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT ${PROJECT_NAME}_Runtime
          NAMELINK_COMPONENT ${PROJECT_NAME}_Development
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT ${PROJECT_NAME}_Development
)

install(DIRECTORY
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN "*.in" EXCLUDE
  PATTERN "*.hpp"
)

install(EXPORT SerialMainTargets
  FILE SerialMainTargets.cmake
  NAMESPACE serial::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
