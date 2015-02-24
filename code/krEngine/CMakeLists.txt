get_filename_component(BIELD_PROJECT_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)

set(FILES
  # Note: the precompiled header should always come first.
  pch.cpp
  pch.h
  common.h
  implementation/common.cpp

  # rendering
  rendering.h
  rendering/drawableBase.h
  rendering/renderExtractor.h
  rendering/implementation/renderExtractor.cpp
  rendering/renderer.h
  rendering/implementation/renderer.cpp
  rendering/debugRenderer.h
  rendering/implementation/debugRenderer.cpp
)

bield_project(
  LIBRARY SHARED
  PCH pch.h
  DEFINE_SYMBOL KR_ENGINE_DLL_EXPORT
  FILES ${FILES}
)

# Put this project in the "engine" folder.
set_target_properties(${BIELD_PROJECT_NAME} PROPERTIES FOLDER engine)

## ezEngine
## ========

set(ezEngine_POST_BUILD_COPY_DLLS ${BIELD_PROJECT_NAME})
find_package(ezEngine REQUIRED COMPONENTS ezThirdParty ezFoundation ezCore ezCoreUtils ezSystem)

## OpenGL and GLEW
## ===============
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

## Putting it all together: include directories and link libraries
## ===============================================================

include_directories(${ezEngine_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
target_link_libraries(${BIELD_PROJECT_NAME} ${ezEngine_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
