include(kr_set_pch)
include(kr_mirror_source_tree)

file(GLOB_RECURSE SOURCES *.h *.inl *.cpp)
kr_set_pch("krEngineTests/pch.h" "pch.cpp" ${SOURCES})
kr_mirror_source_tree("${CMAKE_CURRENT_LIST_DIR}" ${SOURCES})

add_executable(krEngineTests ${SOURCES})
set_target_properties(krEngineTests PROPERTIES
                      FOLDER tests)

target_include_directories(krEngineTests PUBLIC ../../code)
target_include_directories(krEngineTests PUBLIC ..)

find_package(OpenGL REQUIRED)
if(OPENGL_INCLUDE_DIR)
  # Needed on non-windows platforms (I think).
  target_include_directories(krEngineTests PUBLIC ${OPENGL_INCLUDE_DIR})
endif()

target_link_libraries(krEngineTests
                      krEngine
                      glew
                      ezThirdParty ezFoundation ezCore ezCoreUtils ezSystem
                      ${OPENGL_LIBRARIES})

if(MSVC)
  set_target_properties(krEngineTests PROPERTIES COMPILE_FLAGS "/GR")
endif()
