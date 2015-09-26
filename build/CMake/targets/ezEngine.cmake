
set(EZ_COMPONENTS ezThirdParty ezFoundation ezCore ezCoreUtils ezSystem ezTestFramework)

set(EZ_SUFFIX_DEBUG          "-debug")
set(EZ_SUFFIX_MINSIZEREL     "-minsize")
set(EZ_SUFFIX_RELWITHDEBINFO "-reldeb")
set(EZ_SUFFIX_RELEASE        "")
foreach(EZ_COMPONENT ${EZ_COMPONENTS})
  # IMPORTED library
  # Specifies where the .lib and .dll files are located at.
  add_library(IMPORTED_${EZ_COMPONENT} SHARED IMPORTED GLOBAL)
  foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER "${CONFIG}" CONFIG)
    set_property(TARGET IMPORTED_${EZ_COMPONENT} PROPERTY IMPORTED_LOCATION_${CONFIG} "${KREPEL_DIR}/bin/${EZ_COMPONENT}${EZ_SUFFIX_${CONFIG}}.dll")
    set_property(TARGET IMPORTED_${EZ_COMPONENT} PROPERTY IMPORTED_IMPLIB_${CONFIG}   "${KREPEL_DIR}/lib/${EZ_COMPONENT}${EZ_SUFFIX_${CONFIG}}.lib")
  endforeach()
  # INTERFACE library
  # Note: Only INTERFACE libraries can have "include_directories".
  add_library(${EZ_COMPONENT} INTERFACE)
  target_link_libraries(${EZ_COMPONENT} INTERFACE IMPORTED_${EZ_COMPONENT})
  target_include_directories(${EZ_COMPONENT} INTERFACE "${KREPEL_DIR}/code")
endforeach()
add_library(ezEngine INTERFACE)
target_link_libraries(ezEngine INTERFACE ${EZ_COMPONENTS})
