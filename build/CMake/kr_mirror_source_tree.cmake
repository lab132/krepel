
# TODO
# kr_mirror_source_tree(<dir> [source-file]...)
# dir: The absolute path to where the source files are located in.
# source-file: Source file path. Must be an absolute path.
#
# Example:
#   set(SOURCES "C:/foo/bar/main.cpp" "C:/foo/baz/foo.cpp" "C:/foo/baz/foo.h")
#   kr_mirror_source_tree("C:/foo" ${SOURCES})
function(kr_mirror_source_tree BASE_DIR)
  foreach(SOURCE_FILE ${ARGN})
    file(RELATIVE_PATH SOURCE_DIR "${BASE_DIR}" "${SOURCE_FILE}")
    get_filename_component(SOURCE_DIR "${SOURCE_DIR}" DIRECTORY)
    string(REPLACE "/" "\\" GROUP "${SOURCE_DIR}")
    source_group("${GROUP}" FILES "${SOURCE_FILE}")
  endforeach()
endfunction()
