
set(KREPEL_INSTALL_TARGETS_FILE "${KREPEL_DIR}/build/CMake/targets/krepel.cmake"
    CACHE PATH "The filepath of the krepel install targets script")
mark_as_advanced(KREPEL_INSTALL_TARGETS_FILE)

# Clear the file.
file(WRITE "${KREPEL_INSTALL_TARGETS_FILE}"
     "# This file was generated during config-time of krepel.\n"
     "# Needs a valid KREPEL_DIR CMake variable.\n")

function(kr_install_target TARGET_NAME)
  get_target_property(LIBS ${TARGET_NAME} LINK_LIBRARIES)
  file(APPEND "${KREPEL_INSTALL_TARGETS_FILE}"
       "\n# Target: ${TARGET_NAME}\n"
       "add_library(IMPORTED_${TARGET_NAME} SHARED IMPORTED GLOBAL)\n")
  foreach(CFG ${KREPEL_INSTALL_CONFIGURATIONS})
    string(TOUPPER ${CFG} CFG_UPPER)
    file(APPEND "${KREPEL_INSTALL_TARGETS_FILE}"
         "set_property(TARGET IMPORTED_${TARGET_NAME} PROPERTY IMPORTED_LOCATION_${CFG_UPPER} \"\${KREPEL_DIR}/bin/${TARGET_NAME}${CMAKE_${CFG_UPPER}_POSTFIX}.dll\")\n"
         "set_property(TARGET IMPORTED_${TARGET_NAME} PROPERTY IMPORTED_IMPLIB_${CFG_UPPER}   \"\${KREPEL_DIR}/lib/${TARGET_NAME}${CMAKE_${CFG_UPPER}_POSTFIX}.lib\")\n")
  endforeach()
  file(APPEND "${KREPEL_INSTALL_TARGETS_FILE}"
       "add_library(${TARGET_NAME} INTERFACE)\n"
       "target_link_libraries(${TARGET_NAME} INTERFACE IMPORTED_${TARGET_NAME} ${LIBS})\n"
       "target_include_directories(${TARGET_NAME} INTERFACE \"\${KREPEL_DIR}/code\")\n")
endfunction()
