
function(kr_client_copy_dlls CLIENT_DIR)
  if(NOT KREPEL_DIR)
    message("Unable to copy krepel dlls without KREPEL_DIR.")
    return()
  endif()

  set(desc "Whether to copy krepel DLLs to client directory.")
  set(KREPEL_CLIENT_COPY_DLLS ON CACHE BOOL "${desc}")
  if(KREPEL_CLIENT_COPY_DLLS)
    file(INSTALL "${KREPEL_DIR}/bin/"
         DESTINATION "${CLIENT_DIR}"
         FILES_MATCHING
         PATTERN "kr*Launcher.exe"
         PATTERN "*.dll"
         PATTERN "*.pdb")
    set(KREPEL_CLIENT_COPY_DLLS OFF CACHE BOOL "${desc}" FORCE)
  endif()
endfunction()
