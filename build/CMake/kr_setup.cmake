# General configuration script.

macro(kr_setup)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

  # Use at least C++11
  set(CMAKE_CXX_STANDARD 11)

  set(CMAKE_DEBUG_POSTFIX          "-debug"   CACHE STRING "e.g. krepel-debug.dll")
  set(CMAKE_MINSIZEREL_POSTFIX     "-minsize" CACHE STRING "e.g. krepel-minsize.dll")
  set(CMAKE_RELWITHDEBINFO_POSTFIX "-reldeb"  CACHE STRING "e.g. krepel-reldeb.dll")
  set(CMAKE_RELEASE_POSTFIX        ""         CACHE STRING "e.g. krepel.dll (no postfix)")

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
  foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${CONFIG} CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_SOURCE_DIR}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_SOURCE_DIR}/lib")
  endforeach()
endmacro()
