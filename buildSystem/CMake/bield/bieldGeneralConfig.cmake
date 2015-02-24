
### Include guard.
################################################################################
if(BIELD_GENERAL_CONFIG_INCLUDED)
  return()
endif()
set(BIELD_GENERAL_CONFIG_INCLUDED true)


### General configuration.
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(BIELD_OUTPUT_DIR "${CMAKE_BINARY_DIR}/Output" CACHE PATH
    "The directory in which to create the Bin and Lib folders in.")
set(BIELD_OUTPUT_DIR_BIN "${BIELD_OUTPUT_DIR}/Bin")
set(BIELD_OUTPUT_DIR_LIB "${BIELD_OUTPUT_DIR}/Lib")

set(BIELD_FILE_TEMPLATE_DIR "${CMAKE_MODULE_PATH}/templates" CACHE STRING
    "Directory containing all file templates used to generate new files listed in a CMakeLists.txt that do not yet exist on the filesystem.")
set(BIELD_COMPILE_WITH_HIGHEST_WARNING_LEVEL OFF CACHE BOOL
    "Whether to use the highest warning level when compiling. Good luck with that.")
set(BIELD_CREATE_MISSING_FILES ON CACHE BOOL
    "Whether to create files found in the bield_project's FILES variable that are missing from the file system. Does not work for the GLOB option, obviously...")
set(BIELD_USE_PCH ON CACHE BOOL
    "Whether to use precompiled headers or not. The CMake utility 'cotire' is used to generate one if none is explicitly specified to bield_project's PCH variable.")
set(BIELD_USE_UNITY_BUILDS OFF CACHE BOOL
    "Whether to create unity builds using cotire (the CMake utility script).")

mark_as_advanced(BIELD_FILE_TEMPLATE_DIR
                 BIELD_CREATE_MISSING_FILES)

# Set the default build type.
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()
