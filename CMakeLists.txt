#cmake

# our dependency 'cotire' needs at least this version
cmake_minimum_required(VERSION 2.8.6)

# name of the entire project
project("krepel")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/buildSystem/CMake")
include(krepelUtilities)

# the sub-directory 'code' contains all source code.
add_subdirectory("code") # execute CMake code in code/CMakeLists.txt
