include(kr_set_pch)
include(kr_mirror_source_tree)

# Source Files
# ============
file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)

# Target Setup
# ============
add_executable(krGameLauncher ${SOURCES})
set_target_properties(krGameLauncher PROPERTIES
                      FOLDER engine)
target_include_directories(krGameLauncher PUBLIC ..)
kr_set_pch(krGameLauncher "pch.h")
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

# Dependencies
# ============

target_link_libraries(krGameLauncher krEngine)

# Install Target
# ==============
include(kr_install_target)
kr_install_target(krGameLauncher)
