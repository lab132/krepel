
function(_kr_target_make_launchable_msvc TARGET_NAME KREPEL_GAME_LAUNCHER)
  configure_file("${KREPEL_DIR}/build/CMake/launchableGame.vcxproj.user.in"
                 "${CMAKE_BINARY_DIR}/code/${TARGET_NAME}/${TARGET_NAME}.vcxproj.user"
                 @ONLY)
endfunction()

function(kr_target_make_launchable TARGET_NAME)
  find_program(KREPEL_GAME_LAUNCHER
               krGameLauncher
               HINTS ${CMAKE_SOURCE_DIR}/bin)
  if(MSVC)
    _kr_target_make_launchable_msvc(${TARGET_NAME} ${KREPEL_GAME_LAUNCHER})
  else()
    message(WARNING "Making a target launchable is not supported on this platform.")
  endif()
endfunction()
