
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_DEBUG_POSTFIX          "-debug"   CACHE STRING "e.g. krepel-debug.dll")
set(CMAKE_MINSIZEREL_POSTFIX     "-minsize" CACHE STRING "e.g. krepel-minsize.dll")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldeb"  CACHE STRING "e.g. krepel-reldeb.dll")
set(CMAKE_RELEASE_POSTFIX        ""         CACHE STRING "e.g. krepel.dll (no postfix)")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${KREPEL_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${KREPEL_LIB_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${KREPEL_LIB_DIR}")
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CONFIG} CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${KREPEL_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${KREPEL_LIB_DIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${KREPEL_LIB_DIR}")
endforeach()

function(kr_mirror_source_tree BASE_DIR)
  foreach(SOURCE_FILE ${ARGN})
    file(RELATIVE_PATH SOURCE_DIR "${BASE_DIR}" "${SOURCE_FILE}")
    get_filename_component(SOURCE_DIR "${SOURCE_DIR}" DIRECTORY)
    string(REPLACE "/" "\\" GROUP "${SOURCE_DIR}")
    source_group("${GROUP}" FILES "${SOURCE_FILE}")
  endforeach()
endfunction()

function(kr_set_pch PCH_H PCH_CPP)
  if(NOT MSVC)
    return()
  endif()

  get_filename_component(PCH_H "${PCH_H}" ABSOLUTE)
  get_filename_component(PCH_CPP "${PCH_CPP}" ABSOLUTE)
  get_filename_component(PCH_NAME "${PCH_CPP}" NAME_WE)
  set(PCH_PCH "${CMAKE_CURRENT_BINARY_DIR}/${PCH_NAME}.pch")
  set(SOURCES ${ARGN})
  list(REMOVE_ITEM SOURCES "${PCH_H}" "${PCH_CPP}")
  set_source_files_properties(${SOURCES} PROPERTIES COMPILE_FLAGS "/Yu\"${PCH_H}\" /Fp\"${PCH_PCH}\" /FI\"${PCH_H}\""
                                                    OBJECT_OUTPUTS "${PCH_PCH}")
  set_source_files_properties("${PCH_CPP}" PROPERTIES COMPILE_FLAGS "/Yc\"${PCH_H}\" /Fp\"${PCH_PCH}\""
                                                      OBJECT_DEPENDS "${PCH_PCH}")
endfunction()

include(krepel.${CMAKE_SYSTEM_NAME})
