#cmake

set(KR_PROJECT_NAME "application")
set(KR_SOURCE_FILES
    "pch.h"
    "pch.cpp"
    "config.h"
    "implementation/main.cpp"
)

kr_project(
  ${KR_PROJECT_NAME}
  EXECUTABLE
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
    krEngine
)
