get_filename_component(BIELD_PROJECT_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)

set(FILES
  # Note: the precompiled header should always come first.
  pch.cpp
  pch.h

  main.cpp

  # General C++ feature tests.
  sanityTests.cpp

  # Regular tests.
  test_referenceCounting.cpp
  test_texture.cpp
  test_sprite.cpp
  test_shader.cpp
  test_vertexBuffer.cpp
)

bield_project(
  EXECUTABLE
  PCH pch.h
  FILES ${FILES}
)

# Link krEngine
target_link_libraries(${BIELD_PROJECT_NAME} krEngine)

# Put this project in the "tests" folder.
set_target_properties(${BIELD_PROJECT_NAME} PROPERTIES FOLDER tests)

## ezEngine
## ========

set(ezEngine_POST_BUILD_COPY_DLLS ${BIELD_PROJECT_NAME})
find_package(ezEngine
    REQUIRED COMPONENTS
        ezThirdParty
        ezFoundation
        ezCore
        ezCoreUtils
        ezTestFramework)

## OpenGL and GLEW
## ===============
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

## Putting it all together: include directories and link libraries
## ===============================================================

include_directories(${ezEngine_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIR})
target_link_libraries(${BIELD_PROJECT_NAME} ${ezEngine_LIBRARIES} ${OPENGL_LIBRARIES} ${GLEW_LIBRARY})
