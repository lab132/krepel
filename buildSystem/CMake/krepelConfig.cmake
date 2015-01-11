#cmake

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(KR_USE_PCH ON CACHE BOOL
    "Whether to use precompiled headers (if available) or not.")
set(KR_VERBOSITY 0 CACHE STRING
    "Higher values produce more verbose output.")
set(KR_OUTPUT_DIR "${CMAKE_BINARY_DIR}" CACHE PATH
    "The directory in which to create the bin and lib folders in.")
set(KR_LOG_PREFIX "[kr]" CACHE STRING
    "String used to prefix all log messages. Useful to quickly identify out log messages.")
set(KR_DISABLED_WARNINGS "" CACHE STRING
    "List of disabled warnings")
set(KR_FILE_TEMPLATE_DIR "${CMAKE_MODULE_PATH}/templates" CACHE STRING
    "Directory containing all file templates used to generate new files listed in a CMakeLists.txt that does not yet exist on the filesystem.")
set(KR_SHOW_VERBOSITY_IN_LOG OFF CACHE BOOL
    "Whether to show verbosity of a log message or not.")
set(KR_STRICT_WARNINGS OFF CACHE BOOL
    "Whether to use the highest warning level.")

set(KR_COMPILER_SETTINGS_ALL     "" CACHE STRING "Compiler settings used in all builds")
set(KR_COMPILER_SETTINGS_RELEASE "" CACHE STRING "Compiler settings used in release builds only")
set(KR_COMPILER_SETTINGS_DEBUG   "" CACHE STRING "Compiler settings used in debug builds only")
set(KR_LINKER_SETTINGS_ALL       "" CACHE STRING "Linker settings used in all builds")
set(KR_LINKER_SETTINGS_RELEASE   "" CACHE STRING "Linker settings used in release builds only")
set(KR_LINKER_SETTINGS_DEBUG     "" CACHE STRING "Linker settings used in debug builds only")

mark_as_advanced(KR_VERBOSITY
                 KR_LOG_PREFIX
                 KR_DISABLED_WARNINGS
                 KR_FILE_TEMPLATE_DIR
                 KR_SHOW_VERBOSITY_IN_LOG

                 KR_COMPILER_SETTINGS_ALL
                 KR_COMPILER_SETTINGS_RELEASE
                 KR_COMPILER_SETTINGS_DEBUG
                 KR_LINKER_SETTINGS_ALL
                 KR_LINKER_SETTINGS_RELEASE
                 KR_LINKER_SETTINGS_DEBUG)

set(KR_OUTPUT_DIR_BIN "${KR_OUTPUT_DIR}/bin")
set(KR_OUTPUT_DIR_LIB "${KR_OUTPUT_DIR}/lib")

# the following is a modified and stripped version of
# CMAKE_GeneralConfig.txt taken from the ezEngine project.

## other configuration
#######################################################################

# setthe default build type
if(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

#########################################################################################
## Detects the current platform

set(KR_PLATFORM_PREFIX "")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Windows
  kr_log(1 "Platform is Windows (KR_BUILDSYSTEM_PLATFORM_WINDOWS)")
  set(KR_BUILDSYSTEM_PLATFORM_WINDOWS ON)
  set(KR_PLATFORM_PREFIX "Win")
  
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CURRENT_OSX_VERSION)
  # OS X
  kr_log(1 "Platform is OS X (KR_BUILDSYSTEM_PLATFORM_OSX, KR_BUILDSYSTEM_PLATFORM_POSIX)")
  set(KR_BUILDSYSTEM_PLATFORM_OSX ON)
  set(KR_BUILDSYSTEM_PLATFORM_POSIX ON)
  set(KR_PLATFORM_PREFIX "Osx")
  
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  # Linux
  kr_log(1 "Platform is Linux (KR_BUILDSYSTEM_PLATFORM_LINUX, KR_BUILDSYSTEM_PLATFORM_POSIX)")  
  set(KR_BUILDSYSTEM_PLATFORM_LINUX ON)
  set(KR_BUILDSYSTEM_PLATFORM_POSIX ON)
  set(KR_PLATFORM_PREFIX "Linux")
  
else()
  kr_fatal("Platform '${CMAKE_SYSTEM_NAME}' is not supported! Please extend krepelConfig.cmake.")
endif()


#########################################################################################
## Detects the current build-system / generator

set(KR_BUILDSYSTEM_PREFIX "")

if(KR_BUILDSYSTEM_PLATFORM_WINDOWS)
  # Supported windows generators
  if(MSVC)
    # Visual Studio (All VS generators define MSVC)
    kr_log(1 "Buildsystem is MSVC (KR_BUILDSYSTEM_MSVC)")
    set(KR_BUILDSYSTEM_MSVC ON)
    set(KR_BUILDSYSTEM_PREFIX "Vs")
    set(KR_BUILDSYSTEM_CONFIGURATION $<CONFIGURATION>)
  else()
    kr_fatal("Generator '${CMAKE_GENERATOR}' is not supported on Windows! Please extend krepelConfig.cmake.")
  endif()
  
elseif(KR_BUILDSYSTEM_PLATFORM_OSX)
  # Supported OSX generators
  if(CMAKE_GENERATOR STREQUAL "Xcode")
    # XCODE
    kr_log(1 "Buildsystem is Xcode (KR_BUILDSYSTEM_XCODE)")
    set(KR_BUILDSYSTEM_XCODE ON)
    set(KR_BUILDSYSTEM_PREFIX "Xcode")
    set(KR_BUILDSYSTEM_CONFIGURATION $<CONFIGURATION>)
  elseif(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    # Unix Makefiles (for QtCreator etc.)
    kr_log(1 "Buildsystem is Make (KR_BUILDSYSTEM_MAKE)")
    set(KR_BUILDSYSTEM_MAKE ON)
    set(KR_BUILDSYSTEM_PREFIX "Make")
    set(KR_BUILDSYSTEM_CONFIGURATION ${CMAKE_BUILD_TYPE})
  else()
    kr_fatal("Generator '${CMAKE_GENERATOR}' is not supported on OS X! Please extend krepelConfig.cmake.")
  endif()
  
elseif(KR_BUILDSYSTEM_PLATFORM_LINUX)
  if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
    # Unix Makefiles (for QtCreator etc.)
    kr_log(1 "Buildsystem is Make (KR_BUILDSYSTEM_MAKE)")
    set(KR_BUILDSYSTEM_MAKE ON)
    set(KR_BUILDSYSTEM_PREFIX "Make")
    set(KR_BUILDSYSTEM_CONFIGURATION ${CMAKE_BUILD_TYPE})
  else()
    kr_fatal("Generator '${CMAKE_GENERATOR}' is not supported on Linux! Please extend krepelConfig.cmake.")
  endif()
  
else()
  kr_fatal("Platform '${CMAKE_SYSTEM_NAME}' has not setup the supported generators. Please extend krepelConfig.cmake.")
endif()

#########################################################################################
## Detects the current compiler

set(KR_COMPILER_POSTFIX "")

if(KR_BUILDSYSTEM_MSVC)
  # Visual Studio Compiler
  kr_log(1 "Compiler is MSVC (KR_BUILDSYSTEM_COMPILER_MSVC)")
  set(KR_BUILDSYSTEM_COMPILER_MSVC ON)
  
  if(MSVC12)
    kr_log(1 "Compiler is Visual Studio 120 (2013) (KR_BUILDSYSTEM_COMPILER_MSVC_120)")
    set(KR_BUILDSYSTEM_COMPILER_MSVC_120 ON)
    set(KR_COMPILER_POSTFIX "120")
  elseif(MSVC11)
    kr_log(1 "Compiler is Visual Studio 110 (2012) (KR_BUILDSYSTEM_COMPILER_MSVC_110)")
    set(KR_BUILDSYSTEM_COMPILER_MSVC_110 ON)
    set(KR_COMPILER_POSTFIX "110")
  elseif(MSVC10)
    kr_log(1 "Compiler is Visual Studio 100 (2010) (KR_BUILDSYSTEM_COMPILER_MSVC_100)")
    set(KR_BUILDSYSTEM_COMPILER_MSVC_100 ON)
    set(KR_COMPILER_POSTFIX "100")
  else()
    kr_fatal("Compiler for generator '${CMAKE_GENERATOR}' is not supported on MSVC! Please extend krepelConfig.cmake.")
  endif()
  
elseif(KR_BUILDSYSTEM_PLATFORM_OSX)
  # Currently all are clang by default.
  # We should probably make this more idiot-proof in case someone actually changes the compiler to gcc.
  kr_log(1 "Compiler is clang (KR_BUILDSYSTEM_COMPILER_CLANG)")  
  set(KR_BUILDSYSTEM_COMPILER_CLANG ON)
  set(KR_COMPILER_POSTFIX "Clang")
  
elseif(KR_BUILDSYSTEM_PLATFORM_LINUX)
  # Currently all are gcc by default. See OSX comment.
  kr_log(1 "Compiler is gcc (KR_BUILDSYSTEM_COMPILER_GCC)")  
  set(KR_BUILDSYSTEM_COMPILER_GCC ON)
  set(KR_COMPILER_POSTFIX "Gcc")
  
else()
  kr_fatal("Compiler for generator '${CMAKE_GENERATOR}' is not supported on '${CMAKE_SYSTEM_NAME}'. Please extend krepelConfig.cmake.")
endif()


#########################################################################################
## Detects the current architecture

set(KR_ARCHITECTURE_POSTFIX "")

if(KR_BUILDSYSTEM_PLATFORM_WINDOWS AND KR_BUILDSYSTEM_COMPILER_MSVC)
  # Detect 64-bit builds for MSVC.
  if(CMAKE_CL_64)
    kr_log(1 "Platform is 64-Bit (KR_BUILDSYSTEM_PLATFORM_64BIT)")
    set(KR_BUILDSYSTEM_PLATFORM_64BIT ON)
    set(KR_ARCHITECTURE_POSTFIX "64")
  else()
    kr_log(1 "Platform is 32-Bit (KR_BUILDSYSTEM_PLATFORM_32BIT)")
    set(KR_BUILDSYSTEM_PLATFORM_32BIT ON)
    set(KR_ARCHITECTURE_POSTFIX "32")
  endif()
  
elseif(KR_BUILDSYSTEM_PLATFORM_OSX AND KR_BUILDSYSTEM_COMPILER_CLANG)
  # OS X always has 32/64 bit support in the project files and the user switches on demand.
  # However, we do not support 32 bit with our current build configuration so we throw an error on 32-bit systems.
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    kr_log(1 "Platform is 64-Bit (KR_BUILDSYSTEM_PLATFORM_64BIT)")
    set(KR_BUILDSYSTEM_PLATFORM_64BIT ON)
    set(KR_ARCHITECTURE_POSTFIX "64")
  else()
    kr_fatal("32-Bit is not supported on OS X!")
  endif()
  
elseif(KR_BUILDSYSTEM_PLATFORM_LINUX AND KR_BUILDSYSTEM_COMPILER_GCC)
  # Detect 64-bit builds for Linux, no other way than checking CMAKE_SIZEOF_VOID_P.
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    kr_log(1 "Platform is 64-Bit (KR_BUILDSYSTEM_PLATFORM_64BIT)")
    set(KR_BUILDSYSTEM_PLATFORM_64BIT ON)
    set(KR_ARCHITECTURE_POSTFIX "64")
  else()
    kr_log(1 "Platform is 32-Bit (KR_BUILDSYSTEM_PLATFORM_32BIT)")
    set(KR_BUILDSYSTEM_PLATFORM_32BIT ON)
    set(KR_ARCHITECTURE_POSTFIX "32")
  endif()
  
else()
  kr_fatal("Architecture could not be determined. Please extend krepelConfig.cmake.")
endif()

## tell cmake where to build our stuff to
#######################################################################

set(KR_OUTPUT_LIB_DEBUG          "${KR_OUTPUT_DIR_LIB}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}Debug${KR_ARCHITECTURE_POSTFIX}")
set(KR_OUTPUT_LIB_RELEASE        "${KR_OUTPUT_DIR_LIB}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}Release${KR_ARCHITECTURE_POSTFIX}")
set(KR_OUTPUT_LIB_MINSIZE        "${KR_OUTPUT_DIR_LIB}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}MinSize${KR_ARCHITECTURE_POSTFIX}")
set(KR_OUTPUT_LIB_RELWITHDEBINFO "${KR_OUTPUT_DIR_LIB}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}RelWithDebInfo${KR_ARCHITECTURE_POSTFIX}")
                                                       
set(KR_OUTPUT_BIN_DEBUG          "${KR_OUTPUT_DIR_BIN}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}Debug${KR_ARCHITECTURE_POSTFIX}")
set(KR_OUTPUT_BIN_RELEASE        "${KR_OUTPUT_DIR_BIN}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}Release${KR_ARCHITECTURE_POSTFIX}")
set(KR_OUTPUT_BIN_MINSIZE        "${KR_OUTPUT_DIR_BIN}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}MinSize${KR_ARCHITECTURE_POSTFIX}")
set(KR_OUTPUT_BIN_RELWITHDEBINFO "${KR_OUTPUT_DIR_BIN}/${KR_PLATFORM_PREFIX}${KR_BUILDSYSTEM_PREFIX}${KR_COMPILER_POSTFIX}RelWithDebInfo${KR_ARCHITECTURE_POSTFIX}")

kr_log(1 "source dir:     ${CMAKE_SOURCE_DIR}")
kr_log(1 "bin output dir: ${KR_OUTPUT_DIR_BIN}")
kr_log(1 "lib output dir: ${KR_OUTPUT_DIR_LIB}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY                 "${KR_OUTPUT_BIN_DEBUG}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG           "${KR_OUTPUT_BIN_DEBUG}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE         "${KR_OUTPUT_BIN_RELEASE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL      "${KR_OUTPUT_BIN_MINSIZE}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${KR_OUTPUT_BIN_RELWITHDEBINFO}")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY                 "${KR_OUTPUT_LIB_DEBUG}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG           "${KR_OUTPUT_LIB_DEBUG}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE         "${KR_OUTPUT_LIB_RELEASE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL      "${KR_OUTPUT_LIB_MINSIZE}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${KR_OUTPUT_LIB_RELWITHDEBINFO}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY                 "${KR_OUTPUT_LIB_DEBUG}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG           "${KR_OUTPUT_LIB_DEBUG}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE         "${KR_OUTPUT_LIB_RELEASE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL      "${KR_OUTPUT_LIB_MINSIZE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO  "${KR_OUTPUT_LIB_RELWITHDEBINFO}")

## compiler specific settings
#######################################################################

if(KR_BUILDSYSTEM_COMPILER_MSVC)
  # Enable minimal rebuild
  set(KR_COMPILER_SETTINGS_DEBUG "${KR_COMPILER_SETTINGS_DEBUG} /Gm")
  # enable multi-threaded compilation
  set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /MP")
  # disable RTTI
  set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /GR-")
  # use fast floating point model
  set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /fp:fast")
  # enable floating point exceptions
  #set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /fp:except")

  # enable strict warnings
  if(KR_STRICT_WARNINGS)
    set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /W4")
  endif()
  # treat warnings as errors
  set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /WX")

  if(BUILDSYSTEM_PLATFORM_32BIT)
    # enable SSE2 (incompatible with /fp:except)
    set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /arch:SSE2")
    
    if(MSVC10)
      # enable static code analysis, only works on 32 Bit builds
      # (may cause compile errors when combined with Qt, disabled for the time being)
      #set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /analyze")    
      #message (STATUS "Enabling static code analysis.")
    endif()
  endif()
  
  if(MSVC11 OR MSVC12)
    #set(KR_COMPILER_SETTINGS_ALL "${KR_COMPILER_SETTINGS_ALL} /analyze")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /d2Zi+")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /d2Zi+")
  endif()

  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Ox /Ob2 /Oi")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox /Ob2 /Oi")
  
  set(KR_LINKER_SETTINGS_RELEASE "${KR_LINKER_SETTINGS_RELEASE} /INCREMENTAL:NO")
  # Remove unreferenced data (does not work together with incremental build)
  set(KR_LINKER_SETTINGS_RELEASE "${KR_LINKER_SETTINGS_RELEASE} /OPT:REF")
  # Don't know what it does, but Clemens wants it :-) (does not work together with incremental build)
  set(KR_LINKER_SETTINGS_RELEASE "${KR_LINKER_SETTINGS_RELEASE} /OPT:ICF")
  
elseif(KR_BUILDSYSTEM_COMPILER_CLANG)
  # Enable c++11 features
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
elseif(KR_BUILDSYSTEM_COMPILER_GCC)
  # dynamic linking will fail without fPIC (plugins)
  # Wno-enum-compare removes all annoying enum cast warnings
  # std=c++11 is - well needed for c++11.
  # gdwarf-3 will use the old debug info which is compatible with older gdb versions.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wno-enum-compare -std=c++11 -mssse3 -mfpmath=sse -gdwarf-3")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -gdwarf-3")
  
else()
  message (FATAL_ERROR "Not settings are defined for the selected compiler. Please extend krepelConfig.cmake.")
endif()