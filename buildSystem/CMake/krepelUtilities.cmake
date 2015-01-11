#cmake

if(KR_KREPEL_UTILITIES_INCLUDED)
  return()
endif()
set(KR_KREPEL_UTILITIES_INCLUDED ON)

include(CMakeParseArguments)
include(echoTargetProperties)

## logging
########################################################################

# warning ids
set(KR_WARNING_ID_NOT_IMPLEMENTED 0)
set(KR_WARNING_ID_UNPARSED_ARGS   1)

function(kr_generate_log_prefix VERBOSITY OUTPUT_VARIABLE)
  if(KR_SHOW_VERBOSITY_IN_LOG)
    set(${OUTPUT_VARIABLE} "${VERBOSITY})${KR_LOG_PREFIX}" PARENT_SCOPE)
  else()
    set(${OUTPUT_VARIABLE} "${KR_LOG_PREFIX}" PARENT_SCOPE)
  endif()
endfunction()

function(kr_message _VERBOSITY _STATUS _MESSAGE)
  if(NOT _VERBOSITY GREATER KR_VERBOSITY)
    kr_generate_log_prefix(${_VERBOSITY} KR_LOG_PREFIX)
    message(${_STATUS} "${KR_LOG_PREFIX} ${_MESSAGE}")
  endif()
endfunction(kr_message)

function(kr_log _VERBOSITY _MESSAGE)
  if(NOT _VERBOSITY GREATER KR_VERBOSITY)
    kr_generate_log_prefix(${_VERBOSITY} KR_LOG_PREFIX)
    message(STATUS "${KR_LOG_PREFIX} ${_MESSAGE}")
  endif()
endfunction(kr_log)
kr_log(0 "Note: log output for this project is controlled by the advanced cache variable KR_VERBOSITY")

function(kr_error _MESSAGE)
  message(SEND_ERROR "${KR_LOG_PREFIX} ${_MESSAGE}")
endfunction(kr_error)

function(kr_fatal _MESSAGE)
  message(FATAL_ERROR "${KR_LOG_PREFIX} ${_MESSAGE}")
endfunction(kr_fatal)

function(kr_warning _ID _MESSAGE)
  # warnings will only be printed when the global verbosity is 1 or higher
  if(KR_VERBOSITY LESS 1)
    return()
  endif()

  list(FIND "${KR_DISABLED_WARNINGS}" "${_ID}" WARNING_INDEX)
  if(WARNING_INDEX EQUAL -1) # not found
    message(AUTHOR_WARNING "${KR_LOG_PREFIX} ${_MESSAGE}")
  endif()
endfunction(kr_warning)

function(kr_warning_not_implemented)
  kr_warning(${KR_WARNING_ID_NOT_IMPLEMENTED} "[not implemented] ${ARGN}")
endfunction(kr_warning_not_implemented)

function(kr_warning_unparsed_args)
  kr_warning(${KR_WARNING_ID_UNPARSED_ARGS} "[unparsed args] ${ARGN}")
endfunction()

# general config
########################################################################

include(krepelConfig)

# helper functions
########################################################################

function(kr_indent_log_prefix INDENT_STRING)
  set(KR_LOG_PREFIX "${KR_LOG_PREFIX}${INDENT_STRING}" PARENT_SCOPE)
endfunction(kr_indent_log_prefix)

function(kr_unindent_log_prefix UNINDENT_AMOUNT)
  string(LENGTH "${KR_LOG_PREFIX}" LEN)
  math(EXPR LEN "${LEN}-${UNINDENT_AMOUNT}")
  string(SUBSTRING "${KR_LOG_PREFIX}" 0  LEN RESULT)
  set(KR_LOG_PREFIX "${RESULT}" PARENT_SCOPE)
endfunction(kr_unindent_log_prefix)

function(kr_msvc_add_pch_flags PCH)
  kr_indent_log_prefix("(pch)")
  cmake_parse_arguments(PCH "" PREFIX "" ${ARGN})

  get_filename_component(PCH_DIR "${PCH}" DIRECTORY)
  get_filename_component(PCH     "${PCH}" NAME_WE)

  if(PCH_PREFIX)
    set(PCH_PREFIX "${PCH_PREFIX}/")
  endif()

  set(PCH_CREATE_FLAG "/Yc${PCH_PREFIX}${PCH}.h")
  set(PCH_USE_FLAG    "/Yu${PCH_PREFIX}${PCH}.h")

  # if the pch is not on the top-level, prepend the directory
  if(NOT PCH_DIR STREQUAL "")
    set(PCH "${PCH_DIR}/${PCH_NAME}")
  endif()

  kr_log(3 "adding source property '${PCH_CREATE_FLAG}': ${PCH}.cpp")
  # add the necessary compiler flags to pch itself
  set_source_files_properties("${PCH}.cpp" PROPERTIES COMPILE_FLAGS "${PCH_CREATE_FLAG}")

  foreach(SRC_FILE ${ARGN})
    # we ignore the precompiled header and the corresponding .cpp file itself
    if(NOT SRC_FILE STREQUAL "${PCH}.h" AND NOT SRC_FILE STREQUAL "${PCH}.cpp")
      get_filename_component(SRC_EXT "${SRC_FILE}" EXT)
      # only apply the 'use' flag on .cpp files
      if("${SRC_EXT}" STREQUAL ".cpp")
        get_filename_component(SRC_NAME "${SRC_FILE}" NAME_WE)
        kr_log(3 "adding source property '${PCH_USE_FLAG}': ${SRC_FILE}")
        set_source_files_properties ("${SRC_FILE}" PROPERTIES COMPILE_FLAGS "${PCH_USE_FLAG}")
      endif()
    endif()
  endforeach()
endfunction(kr_msvc_add_pch_flags)

function(kr_group_sources_by_file_system)
  kr_indent_log_prefix("(source grouping)")
  foreach(SRC_FILE ${ARGN})
    get_filename_component(SRC_DIR "${SRC_FILE}" DIRECTORY)
    string(REPLACE "/" "\\" SRC_DIR "${SRC_DIR}")
    source_group("${SRC_DIR}" FILES "${SRC_FILE}")
    kr_log(3 "${SRC_DIR} => ${SRC_FILE}")
  endforeach()
endfunction(kr_group_sources_by_file_system)

function(kr_add_sfml TARGET_NAME)
  set(SFML_ROOT "$ENV{SFML_ROOT}" CACHE PATH
      "Path to the (installed) SFML root directory. This variable can be set manually if cmake fails to find SFML automatically.")
  find_package(SFML ${ARGN})
  if(SFML_FOUND)
    include_directories("${SFML_INCLUDE_DIR}")
    target_link_libraries(${TARGET_NAME} ${SFML_LIBRARIES})
  else()
    kr_log(0 "Please specify SFML_ROOT as either a cmake cache variable or an environment variable.")
  endif()

  # copy SFML dlls to output dir as a post-build command
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy_directory
                     "${SFML_ROOT}/bin"
                     $<TARGET_FILE_DIR:${TARGET_NAME}>)

endfunction(kr_add_sfml)

function(kr_add_ezEngine TARGET_NAME)
  cmake_parse_arguments(ezEngine "POST_BUILD_COPY_DLLS" "" "" ${ARGN})
  if(ezEngine_POST_BUILD_COPY_DLLS)
    set(ezEngine_POST_BUILD_COPY_DLLS "${TARGET_NAME}")
  endif()
  find_package(ezEngine ${ezEngine_UNPARSED_ARGUMENTS})

  if(ezEngine_FOUND)
    include_directories("${ezEngine_INCLUDE_DIR}")
    target_link_libraries("${TARGET_NAME}" ${ezEngine_LIBRARIES})
  endif()
endfunction()

function(kr_add_packages TARGET_NAME)
  kr_indent_log_prefix("(packages)")
  set(SUPPORTED_PACKAGES SFML ezEngine)
  cmake_parse_arguments(PKG "" "" "${SUPPORTED_PACKAGES}" ${ARGN})

  if(PKG_UNPARSED_ARGUMENTS)
    kr_warning_unparsed_args("unparsed arguments: ${PKG_UNPARSED_ARGUMENTS}")
  endif()

  if(PKG_SFML)
    kr_log(1 "adding SFML")
    kr_log(2 "args: ${PKG_SFML}")
    kr_add_sfml("${TARGET_NAME}" "${PKG_SFML}")
  endif()

  if(PKG_ezEngine)
    kr_log(1 "adding ezEngine")
    kr_log(2 "args: ${PKG_ezEngine}")
    kr_add_ezEngine("${TARGET_NAME}" "${PKG_ezEngine}")
  endif()

endfunction(kr_add_packages)

# chooses a file template based on the given FILE_EXTENSION
function(kr_get_file_template FILE_EXTENSION OUTPUT_VARIABLE)
  if("${FILE_EXTENSION}" STREQUAL ".h" OR
     "${FILE_EXTENSION}" STREQUAL ".hpp")
    set(${OUTPUT_VARIABLE} "${KR_FILE_TEMPLATE_DIR}/empty.h.template" PARENT_SCOPE)
  elseif("${FILE_EXTENSION}" STREQUAL ".cpp")
    set(${OUTPUT_VARIABLE} "${KR_FILE_TEMPLATE_DIR}/empty.cpp.template" PARENT_SCOPE)
  elseif("${FILE_EXTENSION}" STREQUAL ".inl")
    set(${OUTPUT_VARIABLE} "${KR_FILE_TEMPLATE_DIR}/empty.inl.template" PARENT_SCOPE)
  endif()
endfunction(kr_get_file_template)

function(kr_create_missing_files)
  kr_indent_log_prefix("(creating missing files)")
  foreach(SRC_FILE ${ARGN})
    set(SRC_FILE "${CMAKE_CURRENT_LIST_DIR}/${SRC_FILE}")
    if(NOT EXISTS "${SRC_FILE}")
      get_filename_component(SRC_FILE_EXT "${SRC_FILE}" EXT)
      kr_get_file_template("${SRC_FILE_EXT}" SRC_TEMPLATE)
      kr_log(3 "using template: ${SRC_TEMPLATE}")
      file(READ "${SRC_TEMPLATE}" SRC_TEMPLATE)
      file(WRITE "${SRC_FILE}" "${SRC_TEMPLATE}")
      kr_log(1 "generated: ${SRC_FILE}")
    endif()
  endforeach()
endfunction(kr_create_missing_files)

# project
########################################################################

# signature:
# kr_project(TheProjectName                        # the name of the project.
#            EXECUTABLE|(LIBRARY SHARED|STATIC)    # marks this project as either an executable or a library.
#            [PCH ThePchFileName]                  # the name of the precompiled-header file;
#                                                  # if given, the project will be set up to use a precompiled header. 
#            FILES file0 file1 ... fileN           # all files to include as sources.
#            [PACKAGES (SFML ...)|(ezEngine ...)] # the names and components of the packages this project depends on.
#
# note: SHARED libraries always add_definitions("-DKR_THEPROJECTNAME_EXPORT"), in all upper-case
function(kr_project        PROJECT_NAME)
  set(bool_options         EXECUTABLE)
  set(single_value_options LIBRARY
                           PCH)
  set(multi_value_options  FILES
                           PACKAGES)
  kr_indent_log_prefix("{${PROJECT_NAME}}")
  kr_log(2 "parsing arguments")
  cmake_parse_arguments(PROJECT "${bool_options}" "${single_value_options}" "${multi_value_options}" ${ARGN})

  # error checking
  if(LIB_UNPARSED_ARGUMENTS)
    kr_warning_unparsed_args("unparsed args: ${LIB_UNPARSED_ARGUMENTS}")
  endif()

  if(PROJECT_EXECUTABLE AND PROJECT_LIBRARY)
    kr_error("you must either specify 'EXECUTABLE' or 'LIBRARY <value>' for a kr_project, not both.")
  elseif(NOT PROJECT_EXECUTABLE AND NOT PROJECT_LIBRARY)
    kr_error("either the 'EXECUTABLE' or the 'LIBRARY <value>' must be given to a kr_project.")
  endif()

  if(NOT PROJECT_FILES)
    kr_error("No files specified for project: ${PROJECT_NAME}")
  endif()

  kr_create_missing_files(${PROJECT_FILES})

  # actually start using the given data
  if(PROJECT_LIBRARY) # this project is a library
    kr_log(1 "project is a library (${PROJECT_LIBRARY})")
    add_library(${PROJECT_NAME} ${PROJECT_LIBRARY} ${PROJECT_FILES})
    
    # if this is a shared library (.dll), set a preprocessor definition KR_PROJECTNAME_EXPORT
    if(PROJECT_LIBRARY STREQUAL SHARED)
      set(EXPORT_DEF_NAME "KR_${PROJECT_NAME}_EXPORT")
      # make sure it is in all-caps
      string(TOUPPER "${EXPORT_DEF_NAME}" EXPORT_DEF_NAME)
      kr_log(2 "Adding preprocessor definition: ${EXPORT_DEF_NAME}")
      add_definitions("-D${EXPORT_DEF_NAME}")
    endif()
  elseif(PROJECT_EXECUTABLE)
    kr_log(1 "project is an executable")
    add_executable(${PROJECT_NAME} ${PROJECT_FILES})
  endif()

  if(PROJECT_PCH)
    if(MSVC)
      kr_msvc_add_pch_flags("${PROJECT_PCH}" ${PROJECT_FILES} PREFIX "${PROJECT_NAME}")
    endif()
  endif(PROJECT_PCH)

  kr_group_sources_by_file_system(${PROJECT_FILES})

  kr_add_packages("${PROJECT_NAME}" ${PROJECT_PACKAGES})

  # add compiler flags
  if (KR_COMPILER_SETTINGS_ALL)
    kr_log(2 "setting compiler flags: ${KR_COMPILER_SETTINGS_ALL}")
    set_target_properties (${PROJECT_NAME} PROPERTIES COMPILE_FLAGS ${KR_COMPILER_SETTINGS_ALL})
  endif ()

  # add linker flags
  if (KR_LINKER_SETTINGS_ALL)
    kr_log(2 "setting linker flags (all): ${KR_LINKER_SETTINGS_ALL}")
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG          ${KR_LINKER_SETTINGS_ALL})
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO ${KR_LINKER_SETTINGS_ALL})
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE        ${KR_LINKER_SETTINGS_ALL})
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL     ${KR_LINKER_SETTINGS_ALL})
  endif ()
  if (KR_LINKER_SETTINGS_DEBUG)
    kr_log(2 "setting linker flags (debug): ${KR_LINKER_SETTINGS_DEBUG}")
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_DEBUG          ${KR_LINKER_SETTINGS_DEBUG})
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO ${KR_LINKER_SETTINGS_DEBUG})
  endif ()
  if (KR_LINKER_SETTINGS_RELEASE)
    kr_log(2 "setting linker flags (release): ${KR_LINKER_SETTINGS_RELEASE}")
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_RELEASE    ${KR_LINKER_SETTINGS_RELEASE})
    set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS_MINSIZEREL ${KR_LINKER_SETTINGS_RELEASE})
  endif ()
endfunction(kr_project)
