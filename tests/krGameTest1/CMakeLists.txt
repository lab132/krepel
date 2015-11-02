include(kr_set_pch)
include(kr_mirror_source_tree)

# Source Files
# ============
file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)

# Target Setup
# ============
add_library(krGameTest1 SHARED ${SOURCES})
set_target_properties(krGameTest1 PROPERTIES
                      FOLDER tests)
target_include_directories(krGameTest1 PUBLIC ../../code)
target_include_directories(krGameTest1 PUBLIC ..)
kr_set_pch(krGameTest1 "pch.h")
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

target_link_libraries(krGameTest1 krEngine)
add_dependencies(krGameTest1 krGameLauncher)
