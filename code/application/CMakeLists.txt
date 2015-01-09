#cmake

set(kr_project_name "application")
kr_project(
  ${kr_project_name}
  EXECUTABLE
  PCH pch.h
  FILES
    "pch.h"
    "pch.cpp"
    "config.h"
    "implementation/main.cpp"
  PACKAGES
    SFML 2 REQUIRED COMPONENTS system window graphics audio
    #EZ_ENGINE REQUIRED COMPONENTS Foundation Core
)
