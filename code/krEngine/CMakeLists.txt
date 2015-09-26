include(kr_set_pch)
include(kr_mirror_source_tree)

file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)
kr_set_pch("pch.h" "pch.cpp" ${SOURCES})
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

add_library(krEngine SHARED ${SOURCES})
set_target_properties(krEngine PROPERTIES
                      FOLDER engine
                      DEFINE_SYMBOL KR_ENGINE_DLL_EXPORT)

target_include_directories(krEngine PUBLIC ..)

find_package(OpenGL REQUIRED)
if(OPENGL_INCLUDE_DIR)
  # Needed on non-windows platforms (I think).
  target_include_directories(krEngine PUBLIC ${OPENGL_INCLUDE_DIR})
endif()

target_link_libraries(krEngine
                      glew
                      ezThirdParty ezFoundation ezCore ezCoreUtils ezSystem
                      ${OPENGL_LIBRARIES})

include(kr_install_target)
kr_install_target(krEngine)
