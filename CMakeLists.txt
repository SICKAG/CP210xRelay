cmake_policy(SET CMP0048 NEW)
project(CP210xRelay VERSION 1.0)
cmake_minimum_required(VERSION 3.14)

add_executable(CP210xRelay src/CP210xRelay.cpp)
target_include_directories(CP210xRelay PRIVATE lib)
add_library(CP210xLib SHARED IMPORTED)

set_target_properties(CP210xLib PROPERTIES
  IMPORTED_IMPLIB "${PROJECT_SOURCE_DIR}/lib/CP210xRuntime.lib"
  IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/CP210xRuntime.dll"
)

add_custom_command (TARGET CP210xRelay POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/lib/CP210xRuntime.dll ${CMAKE_BINARY_DIR}
)

target_link_libraries(CP210xRelay CP210xLib)
