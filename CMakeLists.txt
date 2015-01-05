#cmake
cmake_minimum_required(VERSION 2.8)
project ("krepel")
message (STATUS "Adding SubDirectory 'code'")
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/code)
