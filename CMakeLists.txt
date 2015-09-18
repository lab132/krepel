#cmake

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

# Name of the entire project.
project("krepel")

set(KREPEL_DIR "${CMAKE_SOURCE_DIR}")
set(KREPEL_BIN_DIR "${KREPEL_DIR}/bin" CACHE PATH "Krepel binaries directory (.exe, .dll, .pdb, .so).")
set(KREPEL_LIB_DIR "${KREPEL_DIR}/lib" CACHE PATH "Krepel libraries directory (.lib, .a, .exp, .dylib)")

set(CMAKE_MODULE_PATH "${KREPEL_DIR}/build/CMake/")
include("krepel")

include_directories("code")
add_subdirectory("code")
add_subdirectory("tests")
