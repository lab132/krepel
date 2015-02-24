
### Include guard
################################################################################
if(BIELD_FLAGS_INCLUDED)
  return()
endif()
set(BIELD_FLAGS_INCLUDED ON)

### Functions / Macros
################################################################################

function(bield_apply_linker_flags TARGET_NAME)
  ### Apply linker flags for each configuration.
  ##############################################################################
  if(BIELD_LINKER_FLAGS)
    set_target_properties(${TARGET_NAME} PROPERTIES
      LINK_FLAGS "${BIELD_LINKER_FLAGS}")
    bield_log(2 "Applying linker flags to all configs of '${TARGET_NAME}': ${BIELD_LINKER_FLAGS}")
  endif()
  # Set config specific flags.
  foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${CFG}" CFG)
    if(BIELD_LINKER_FLAGS_${CFG})
      set_target_properties(${TARGET_NAME} PROPERTIES
        LINK_FLAGS "${BIELD_LINKER_FLAGS_${CFG}}")
      bield_log(2 "Applying linker flags to ${cfg} config of '${TARGET_NAME}': ${BIELD_LINKER_FLAGS_${CFG}}")
    endif()
  endforeach()
endfunction(bield_apply_linker_flags)
