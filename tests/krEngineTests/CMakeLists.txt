include(kr_set_pch)
include(kr_mirror_source_tree)

# Source Files
# ============
file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)

# Target Setup
# ============
add_executable(krEngineTests ${SOURCES})
set_target_properties(krEngineTests PROPERTIES
                      FOLDER tests)
target_include_directories(krEngineTests PUBLIC ../../code)
target_include_directories(krEngineTests PUBLIC ..)
kr_set_pch(krEngineTests "pch.h")
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

# Dependencies
# ============

find_package(OpenGL REQUIRED)
if(OPENGL_INCLUDE_DIR)
  # Needed on non-windows platforms (I think).
  target_include_directories(krEngineTests PUBLIC ${OPENGL_INCLUDE_DIR})
endif()

target_link_libraries(krEngineTests
                      krEngine
                      ${OPENGL_LIBRARIES})

# Special Compile Options
# =======================
if(MSVC)
  # Enable RTTI for the tests since Catch requires it.
  target_compile_options(krEngineTests PRIVATE /GR)
endif()
