#cmake

set(KR_PROJECT_NAME "krEngine")
set(KR_SOURCE_FILES
    "pch.h"
    "pch.cpp"
    "common.h"
    "implementation/common.cpp"

    # rendering
    "rendering.h"
    "rendering/drawableBase.h"
    "rendering/renderExtractor.h"
    "rendering/implementation/renderExtractor.cpp"
    "rendering/renderer.h"
    "rendering/implementation/renderer.cpp"
    "rendering/debugRenderer.h"
    "rendering/implementation/debugRenderer.cpp"
)

kr_project(
  ${KR_PROJECT_NAME}
  LIBRARY SHARED
  PCH pch.h
  FILES ${KR_SOURCE_FILES}
  PACKAGES
    SFML 2
      REQUIRED COMPONENTS
        system window graphics
    ezEngine
      REQUIRED COMPONENTS
        ezFoundation ezCore ezSystem
      POST_BUILD_COPY_DLLS
)
