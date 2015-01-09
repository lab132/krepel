#cmake

kr_project(
  "application"
  EXECUTABLE
  PCH pch.h
  FILES
    "pch.h"
    "pch.cpp"
    "config.h"
    "implementation/main.cpp"
)
