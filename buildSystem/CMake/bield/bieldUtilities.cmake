################################################################################
### Note: The most important function of this file is bield_project.         ###
################################################################################

### Include guard.
################################################################################
if(BIELD_UTILITIES_INCLUDED)
  return()
endif()
set(BIELD_UTILITIES_INCLUDED ON)

### Helper functions
################################################################################

function(bield_group_sources_by_file_system)
  bield_indent_log_prefix("(source grouping)")
  foreach(SRC_FILE ${ARGN})
    get_filename_component(SRC_DIR "${SRC_FILE}" DIRECTORY)
    string(REPLACE "/" "\\" SRC_DIR "${SRC_DIR}")
    source_group("${SRC_DIR}" FILES "${SRC_FILE}")
    if(SRC_DIR)
      bield_log(3 "Group '${SRC_DIR}': ${SRC_FILE}")
    else()
      bield_log(3 "Top-level: ${SRC_FILE}")
    endif()
  endforeach()
endfunction(bield_group_sources_by_file_system)

# Chooses a file template based on the given FILE_EXTENSION.
function(bield_get_file_template FILE_EXTENSION OUTPUT_VARIABLE)
  if("${FILE_EXTENSION}" STREQUAL ".h" OR
     "${FILE_EXTENSION}" STREQUAL ".hpp")
    set(${OUTPUT_VARIABLE} "${BIELD_FILE_TEMPLATE_DIR}/empty.h.template" PARENT_SCOPE)
  elseif("${FILE_EXTENSION}" STREQUAL ".cpp")
    set(${OUTPUT_VARIABLE} "${BIELD_FILE_TEMPLATE_DIR}/empty.cpp.template" PARENT_SCOPE)
  elseif("${FILE_EXTENSION}" STREQUAL ".inl")
    set(${OUTPUT_VARIABLE} "${BIELD_FILE_TEMPLATE_DIR}/empty.inl.template" PARENT_SCOPE)
  endif()
endfunction(bield_get_file_template)

function(bield_create_all_missing_files)
  bield_indent_log_prefix("(creating missing files)")
  foreach(SRC_FILE ${ARGN})
    set(SRC_FILE "${CMAKE_CURRENT_LIST_DIR}/${SRC_FILE}")
    if(NOT EXISTS "${SRC_FILE}")
      get_filename_component(SRC_FILE_EXT "${SRC_FILE}" EXT)
      bield_get_file_template("${SRC_FILE_EXT}" SRC_TEMPLATE)
      if(EXISTS "${SRC_TEMPLATE}")
        bield_log(3 "using template: ${SRC_TEMPLATE}")
        file(READ "${SRC_TEMPLATE}" SRC_TEMPLATE)
      else()
        bield_log(3 "template not found: ${SRC_TEMPLATE}")
        set(SRC_TEMPLATE "")
      endif()
      file(WRITE "${SRC_FILE}" "${SRC_TEMPLATE}")
      bield_log(1 "generated: ${SRC_FILE}")
    endif()
  endforeach()
endfunction(bield_create_all_missing_files)

# DIR_PREFIX is something like "Win", or "Linux".
# Note: Makes use of the typical _STRING variables, such as BIELD_GENERATOR_STRING
function(bield_set_output_directories DIR_PREFIX)
  set(BIELD_PLATFORM_NAME "${DIR_PREFIX}${BIELD_ARCHITECTURE_STRING}${BIELD_GENERATOR_STRING}")
  set(BIELD_PLATFORM_NAME "${DIR_PREFIX}${BIELD_ARCHITECTURE_STRING}${BIELD_GENERATOR_STRING}" PARENT_SCOPE)

  bield_log(4 "BIELD_PLATFORM_NAME: ${BIELD_PLATFORM_NAME}")

  # Configuration neutral output directories.
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIELD_OUTPUT_DIR_BIN}/${BIELD_PLATFORM_NAME}" PARENT_SCOPE)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}" PARENT_SCOPE)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}" PARENT_SCOPE)

  bield_log(3 "CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${BIELD_OUTPUT_DIR_BIN}/${BIELD_PLATFORM_NAME}")
  bield_log(3 "CMAKE_LIBRARY_OUTPUT_DIRECTORY: ${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}")
  bield_log(3 "CMAKE_ARCHIVE_OUTPUT_DIRECTORY: ${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}")

  # Iterate over all configuration types and set appropriate output dirs.
  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${cfg}" CFG) # Debug => DEBUG

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG} "${BIELD_OUTPUT_DIR_BIN}/${BIELD_PLATFORM_NAME}${cfg}" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG} "${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}${cfg}" PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG} "${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}${cfg}" PARENT_SCOPE)

    bield_log(3 "CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG}: ${BIELD_OUTPUT_DIR_BIN}/${BIELD_PLATFORM_NAME}${cfg}")
    bield_log(3 "CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG}: ${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}${cfg}")
    bield_log(3 "CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG}: ${BIELD_OUTPUT_DIR_LIB}/${BIELD_PLATFORM_NAME}${cfg}")
  endforeach()
endfunction()

### Project
################################################################################

# Needed for bield_apply_target_flags.
include(bield/bieldFlags)

# signature:
# bield_project(EXECUTABLE|(LIBRARY SHARED|STATIC)  # Marks this project as either an executable or a library.
#               [UNITY_BUILD]                       # Whether to generate a unity build target according to cotire.
#               [PCH PCH.h]                         # The name of the prefix-header files;
#                                                   # if given, the project will be set up to use a precompiled header, regardless of the BIELD_USE_PCH setting.
#               [DEFINE_SYMBOL]                     # SHARED libraries only. Will add a #define such as MYPROJECT_EXPORTS. If omitted, will use "<to-upper>${PROJECT_NAME}</to-upper>_EXPORTS"
#               FILES file0 file1 ... fileN)        # All files to include as sources.
# NOTE: When using the PCH option for the MSVC or Intel C++ compiler, make sure
#       to provide the accompanying .cpp file (e.g. PCH.cpp or stdafx.cpp) as
#       the *first* entry to FILES.
# TODO Use CMAKE_CURRENT_SOURCE_DIR to determine the project name.
function(bield_project)
  set(bool_options         EXECUTABLE
                           UNITY_BUILD)
  set(single_value_options LIBRARY
                           DEFINE_SYMBOL)
  set(multi_value_options  FILES
                           PCH)
  get_filename_component(PROJECT_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
  bield_indent_log_prefix("{${PROJECT_NAME}}")
  bield_log(2 "parsing arguments")
  cmake_parse_arguments(PROJECT "${bool_options}" "${single_value_options}" "${multi_value_options}" ${ARGN})

  ### Error checking.
  ##############################################################################
  if(PROJECT_UNPARSED_ARGUMENTS)
    bield_error_unparsed_args(${PROJECT_UNPARSED_ARGUMENTS})
  endif()

  if(PROJECT_EXECUTABLE AND PROJECT_LIBRARY)
    bield_error("you must either specify 'EXECUTABLE' or 'LIBRARY <value>' for a bield_project, not both.")
  elseif(NOT PROJECT_EXECUTABLE AND NOT PROJECT_LIBRARY)
    bield_error("either the 'EXECUTABLE' or the 'LIBRARY <value>' must be given to a bield_project.")
  endif()

  # Just in case the user did not list the PCH in the FILES option, we append it.
  list(APPEND PROJECT_FILES ${PROJECT_PCH})

  # Remove duplicates to reduce headache.
  list(REMOVE_DUPLICATES PROJECT_FILES)

  # Check to see if any files were specified.
  if(NOT PROJECT_FILES)
    bield_error("No files specified for project: ${PROJECT_NAME}")
  endif()

  # If enabled, create all FILES missing from the file-system.
  if(BIELD_CREATE_MISSING_FILES)
    bield_create_all_missing_files(${PROJECT_FILES})
  endif()

  ### Create the target
  ##############################################################################
  if(PROJECT_LIBRARY)
    bield_log(3 "Creating ${PROJECT_LIBRARY} library target '${PROJECT_NAME}'.")
    add_library(${PROJECT_NAME} ${PROJECT_LIBRARY} ${PROJECT_FILES})
    if(PROJECT_LIBRARY STREQUAL "SHARED")
      # If the DEFINE_SYMBOL option was not given to bield_project,
      # generate one by using the upper-cased PROJECT_NAME.
      if(NOT PROJECT_DEFINE_SYMBOL)
        # For a project name "HelloWorld" => "HELLOWORLD_EXPORTS"
        string(TOUPPER "${PROJECT_NAME}_EXPORTS" PROJECT_DEFINE_SYMBOL)
      endif()
      set_target_properties(${PROJECT_NAME} PROPERTIES
                            DEFINE_SYMBOL ${PROJECT_DEFINE_SYMBOL})
    endif()
  elseif(PROJECT_EXECUTABLE)
    bield_log(3 "Creating executable target '${PROJECT_NAME}'.")
    add_executable(${PROJECT_NAME} ${PROJECT_FILES})
  else()
    bield_error("This should be unreachable code!")
  endif()

  bield_log(3 "Project files: ${PROJECT_FILES}")

  bield_apply_linker_flags(${PROJECT_NAME})
  bield_group_sources_by_file_system(${PROJECT_FILES})

  if(PROJECT_PCH)
    bield_log(1 "Using precompiled headers for ${PROJECT_NAME} (because the PCH option was given to bield_project)")
    set(BIELD_USE_PCH ON PARENT_SCOPE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
      COTIRE_CXX_PREFIX_HEADER_INIT ${PROJECT_PCH})
  endif()

  # if the UNITY_BUILD option as given, override BIELD_USE_UNITY_BUILDS
  if(PROJECT_UNITY_BUILD)
    set(BIELD_USE_UNITY_BUILDS ON PARENT_SCOPE)
  endif()

  set_target_properties(${PROJECT_NAME} PROPERTIES
    # Enable/disable pch.
    COTIRE_ENABLE_PRECOMPILED_HEADER ${BIELD_USE_PCH}
    # Enable/disable unity builds.
    COTIRE_ADD_UNITY_BUILD ${BIELD_USE_UNITY_BUILDS})

  # Invoke cotire on the target.
  cotire(${PROJECT_NAME})
endfunction(bield_project)
