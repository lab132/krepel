# This file was generated during config-time of krepel.
# Needs a valid KREPEL_DIR CMake variable.

# Target: krEngine
add_library(IMPORTED_krEngine SHARED IMPORTED GLOBAL)
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_LOCATION_DEBUG "${KREPEL_DIR}/bin/krEngine-debug.dll")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_IMPLIB_DEBUG   "${KREPEL_DIR}/lib/krEngine-debug.lib")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_LOCATION_RELEASE "${KREPEL_DIR}/bin/krEngine.dll")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_IMPLIB_RELEASE   "${KREPEL_DIR}/lib/krEngine.lib")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_LOCATION_MINSIZEREL "${KREPEL_DIR}/bin/krEngine-minsize.dll")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_IMPLIB_MINSIZEREL   "${KREPEL_DIR}/lib/krEngine-minsize.lib")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${KREPEL_DIR}/bin/krEngine-reldeb.dll")
set_property(TARGET IMPORTED_krEngine PROPERTY IMPORTED_IMPLIB_RELWITHDEBINFO   "${KREPEL_DIR}/lib/krEngine-reldeb.lib")
add_library(krEngine INTERFACE)
target_link_libraries(krEngine INTERFACE IMPORTED_krEngine glew;ezThirdParty;ezFoundation;ezCore;ezCoreUtils;ezSystem;glu32;opengl32)
target_include_directories(krEngine INTERFACE "${KREPEL_DIR}/code")

# Target: krGameLauncher
add_library(IMPORTED_krGameLauncher SHARED IMPORTED GLOBAL)
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_LOCATION_DEBUG "${KREPEL_DIR}/bin/krGameLauncher-debug.dll")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_IMPLIB_DEBUG   "${KREPEL_DIR}/lib/krGameLauncher-debug.lib")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_LOCATION_RELEASE "${KREPEL_DIR}/bin/krGameLauncher.dll")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_IMPLIB_RELEASE   "${KREPEL_DIR}/lib/krGameLauncher.lib")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_LOCATION_MINSIZEREL "${KREPEL_DIR}/bin/krGameLauncher-minsize.dll")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_IMPLIB_MINSIZEREL   "${KREPEL_DIR}/lib/krGameLauncher-minsize.lib")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${KREPEL_DIR}/bin/krGameLauncher-reldeb.dll")
set_property(TARGET IMPORTED_krGameLauncher PROPERTY IMPORTED_IMPLIB_RELWITHDEBINFO   "${KREPEL_DIR}/lib/krGameLauncher-reldeb.lib")
add_library(krGameLauncher INTERFACE)
target_link_libraries(krGameLauncher INTERFACE IMPORTED_krGameLauncher krEngine)
target_include_directories(krGameLauncher INTERFACE "${KREPEL_DIR}/code")
