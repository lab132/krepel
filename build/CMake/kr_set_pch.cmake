
# Set up usage of precompiled headers. MSVC projects only.
# kr_set_pch(<precompiled-header> <precompiled-source> [source-file]...)
# precompiled-header: The name of the header file that will be included by all the translation units (.cpp files).
#                     This must match the argument to the #include directive EXACTLY,
#                     e.g. #include <foo/bar.h> means you have to pass "foo/bar.h" here,
#                     not "foo\bar.h" or the absolute path of bar.h.
# precompiled-source: The translation file that will be used to produce the precompiled header.
#                     Can be either a relative or absolute path.
# source-file:        The list of source files that will be forced to include the precompiled header.
#                     You may list header files here, but that will not have any effect at all.
#                     The <precompiled-source> file itself can be contained as well, but will be ignored.
# Example for a project "foo":
#   set(SOURCES "main.cpp" "foo.h" "foo.cpp" "config.h" "math.h")
#   kr_set_pch("foo/pch.h" "pch.cpp" ${SOURCES})
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
  set_source_files_properties("${PCH_CPP}" PROPERTIES COMPILE_FLAGS "/Yc\"${PCH_H}\" /Fp\"${PCH_PCH}\" /FI\"${PCH_H}\""
                                                      OBJECT_DEPENDS "${PCH_PCH}")
endfunction()
