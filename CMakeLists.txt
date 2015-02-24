#cmake

# our dependency 'cotire' needs at least this version
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

# name of the entire project
project("krepel")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/buildSystem/CMake")

# Set the default output dirs.
set(BIELD_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/output")
set(BIELD_OUTPUT_DIR_BIN "${BIELD_OUTPUT_DIR}/bin")
set(BIELD_OUTPUT_DIR_LIB "${BIELD_OUTPUT_DIR}/lib")

# Include the Bield master script.
include(bield)

# the sub-directory 'code' contains all source code.
add_subdirectory("code") # execute CMake code in code/CMakeLists.txt
