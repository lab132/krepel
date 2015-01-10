#cmake

# Tries to find the ezEngine project.
# Input:
#   EZ_ENGINE_ROOT: The root directory of the ezEngine.
#   Optional:
#     EZ_ENGINE_POST_BUILD_COPY_DLLS: Will add a custom post-build command which will copy the ezEngine DLLs to the output directory of the given target: set(EZ_ENGINE_POST_BUILD_COPY_DLLS MyTargetName)
# Output:
#   EZ_ENGINE_FOUND:        TRUE if all required components were PACKAGES_FOUND
#   EZ_ENGINE_INCLUDE_DIR:  The include directory of the ezEngine sources. Use it like this: include_directories("${EZ_ENGINE_INCLUDE_DIR}")
#   EZ_ENGINE_LIBRARIES:    A list of targets that can be used like this: target_link_libraries(MyTargetName ${EZ_ENGINE_LIBRARIES})
#   EZ_ENGINE_OUTPUT_DIR:   The output directory of the ezEngine.
#   EZ_ENGINE_BIN_DIR:      The directory of the ezEngine binaries (executables and shared libraries, i.e. DLLs).
#   EZ_ENGINE_LIB_DIR:      The directory of the ezEngine static and import libraries (.lib).

find_path(EZ_ENGINE_ROOT
          NAMES Code/Engine Output
          PATHS "$ENV{EZ_ENGINE_ROOT}" "${EZ_ENGINE_ROOT}")

if(EZ_ENGINE_ROOT)
  set(EZ_ENGINE_FOUND TRUE)
endif()

if(NOT EZ_ENGINE_FOUND)
  set(EZ_ENGINE_FIND_MESSAGE "Unable to locate ezEngine. Please specify EZ_ENGINE_ROOT.")
  if(EZ_ENGINE_FIND_REQUIRED)
    message(SEND_ERROR "${EZ_ENGINE_FIND_MESSAGE}")
  else()
    message(STATUS "${EZ_ENGINE_FIND_MESSAGE}")
  endif()
  return()
else()
  message(STATUS "Found ezEngine in: ${EZ_ENGINE_ROOT}")
endif()

set(EZ_ENGINE_INCLUDE_DIR "${EZ_ENGINE_ROOT}/Code/Engine")
set(EZ_ENGINE_OUTPUT_DIR "${EZ_ENGINE_ROOT}/Output")
set(EZ_ENGINE_BIN_DIR "${EZ_ENGINE_ROOT}/Output/Bin")
set(EZ_ENGINE_LIB_DIR "${EZ_ENGINE_ROOT}/Output/Lib")

include("${EZ_ENGINE_ROOT}/Code/exports.cmake")

foreach(COMPONENT ${EZ_ENGINE_FIND_COMPONENTS})
  if(TARGET ${COMPONENT})
    message("Found target for module: ${COMPONENT}")
    list(APPEND EZ_ENGINE_LIBRARIES ${COMPONENT})
  elseif(EZ_ENGINE_FIND_REQUIRED_${COMPONENT})
    set(EZ_ENGINE_FOUND FALSE)
    message(SEND_ERROR "Cannot find required ezEngine module: ${COMPONENT}")
  endif()
endforeach()

if(EZ_ENGINE_POST_BUILD_COPY_DLLS)
  message("NOT IMPLEMENTED: adding post-build command to copy ezEngine DLLs for target: ${EZ_ENGINE_POST_BUILD_COPY_DLLS}")
  #add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  #                 COMMAND ${CMAKE_COMMAND} -E copy_if_newer
  #                 "${EZ_ENGINE_OUTPUT_DIR}/???"
  #                 $<TARGET_FILE_DIR:${TARGET_NAME}>)
endif()



