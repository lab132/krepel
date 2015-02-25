get_filename_component(BIELD_PROJECT_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)

set(FILES
  # Note: the precompiled header should always come first.
  pch.cpp
  pch.h

  ## Common
  ## ======
  common.h
  common/ezArrayPtrExtension.h
  common/ezTypesExtension.h
  common/ezContainersExtension.h
  common/utils.h

  ## ReferenceCounting
  ## =================
  referenceCounting.h

  referenceCounting/refcounted.h
  referenceCounting/refCountedPtr.h

  ## Rendering
  ## =========
  rendering.h

  # Renderer
  rendering/renderer.h
  rendering/implementation/renderer.cpp
  rendering/implementation/extractionBuffer.h
  rendering/implementation/extractionBuffer.cpp

  # Window
  rendering/window.h
  rendering/implementation/window.cpp
  rendering/implementation/openGlContext.inl
  rendering/implementation/windowImpl.h

  # Sprite
  rendering/sprite.h
  rendering/implementation/sprite.cpp

  # Texture
  rendering/texture.h
  rendering/implementation/textureImpl.h
  rendering/implementation/texture.cpp

  # Shaders
  rendering/shader.h
  rendering/implementation/shader.cpp
  rendering/implementation/uploadUniformValue.cpp

  # VertexBuffer
  rendering/vertexBuffer.h
  rendering/implementation/vertexBuffer.cpp
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
find_package(ezEngine
  REQUIRED COMPONENTS
    ezThirdParty
    ezFoundation
    ezCore
    ezCoreUtils
    ezSystem)

## OpenGL and GLEW
## ===============
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

## Putting it all together: include directories and link libraries
## ===============================================================

include_directories(${ezEngine_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
target_link_libraries(${BIELD_PROJECT_NAME} ${ezEngine_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
