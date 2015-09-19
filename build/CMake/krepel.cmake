# General configuration script.

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Use at least C++11
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_DEBUG_POSTFIX          "-debug"   CACHE STRING "e.g. krepel-debug.dll")
set(CMAKE_MINSIZEREL_POSTFIX     "-minsize" CACHE STRING "e.g. krepel-minsize.dll")
set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldeb"  CACHE STRING "e.g. krepel-reldeb.dll")
set(CMAKE_RELEASE_POSTFIX        ""         CACHE STRING "e.g. krepel.dll (no postfix)")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${KREPEL_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${KREPEL_LIB_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${KREPEL_LIB_DIR}")
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER ${CONFIG} CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${KREPEL_BIN_DIR}")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${KREPEL_LIB_DIR}")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${KREPEL_LIB_DIR}")
endforeach()

include(platforms/${CMAKE_SYSTEM_NAME})

include(utils/kr_mirror_source_tree)
include(utils/kr_set_pch)

include(targets/glew)
include(targets/ezEngine)
