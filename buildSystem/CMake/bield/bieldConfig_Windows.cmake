### Include guard
################################################################################
if(BIELD_CONFIG_WINDOWS_INCLUDED)
  return()
endif()
set(BIELD_CONFIG_WINDOWS_INCLUDED ON)

### Set defaults. These may be overridden later within this script.
################################################################################
# We're running on Windows.
set(BIELD_PLATFORM_WINDOWS ON)

# Using MSVC at all?
set(BIELD_COMPILER_MSVC OFF)

# Using MSVC version 100 (2010)?
set(BIELD_COMPILER_MSVC_2010 OFF)

# Using MSVC version 110 (2012)?
set(BIELD_COMPILER_MSVC_2012 OFF)

# Using MSVC version 120 (2013)?
set(BIELD_COMPILER_MSVC_2013 OFF)

# Compiling for a 32 bit architecture?
set(BIELD_ARCHITECTURE_32BIT OFF)

# Compiling for a 64 bit architecture?
set(BIELD_ARCHITECTURE_64BIT OFF)

# String that describes the used compiler.
set(BIELD_COMPILER_STRING "-NOTFOUND")

# String that describes the platform architecture (32/64 bit).
set(BIELD_ARCHITECTURE_STRING "-NOTFOUND")

# String that describes the generator used (e.g. "VisualStudio2013").
set(BIELD_GENERATOR_STRING "-NOTFOUND")

# Info that will be logged once all information is gathered.
set(INFO "Platform: Windows")

### Detect architecture.
################################################################################
if(MSVC AND CMAKE_CL_64 OR CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BIELD_ARCHITECTURE_64BIT ON)
  set(BIELD_ARCHITECTURE_STRING "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(BIELD_ARCHITECTURE_32BIT ON)
  set(BIELD_ARCHITECTURE_STRING "32")
else()
  bield_fatal("Enable to determine size of void* on Windows!")
endif()

set(INFO "${INFO} ${BIELD_ARCHITECTURE_STRING} bit")

### Detect generator.
################################################################################
if(CMAKE_GENERATOR MATCHES "Visual Studio")
  # All VS generators define MSVC
  if(NOT MSVC)
    bield_fatal("Generator is Visual Studio, but MSVC is not defined!")
  endif()

  set(BIELD_COMPILER_MSVC ON)
  set(BIELD_COMPILER_STRING "MSVC")

  # Known MSVC version numbers are:
  # 1200 = VS  6.0
  # 1300 = VS  7.0
  # 1310 = VS  7.1
  # 1400 = VS  8.0
  # 1500 = VS  9.0
  # 1600 = VS 10.0 (2010)
  # 1700 = VS 11.0 (2012)
  # 1800 = VS 12.0 (2013)

  if(MSVC_VERSION LESS 1600)
    bield_fatal("The given generator is not supported (too old)!")
  elseif(MSVC_VERSION GREATER 1800)
    bield_warning_compiler_version("Newer compiler versions than VS 12.0 (2013) might not be working properly. Use at your own risk.")
  endif()

  if(MSVC12) # 2013
    set(BIELD_COMPILER_MSVC_2013 ON)
    set(BIELD_GENERATOR_STRING "VisualStudio2013")
  elseif(MSVC11) # 2012
    set(BIELD_COMPILER_MSVC_2012 ON)
    set(BIELD_GENERATOR_STRING "VisualStudio2012")
  elseif(MSVC10) # 2010
    set(BIELD_COMPILER_MSVC_2010 ON)
    set(BIELD_GENERATOR_STRING "VisualStudio2010")
  else()
    bield_error("This should be unreachable code!")
  endif()

else()
  bield_fatal("Generator '${CMAKE_GENERATOR}' is not supported on Windows!.")
endif()
set(INFO "${INFO} using ${BIELD_GENERATOR_STRING}")

### Detect compiler.
################################################################################
# On Windows, only MSVC is supported for now.
if(NOT MSVC)
  bield_fatal("Unsupported compiler. Only MSVC is supported on Windows.")
endif()

set(INFO "${INFO} and ${BIELD_COMPILER_STRING}")

### Process collected data.
################################################################################
bield_log(1 "${INFO}.")

# Depends on the _STRING variables.
bield_set_output_directories("Win")

### Global compiler flags.
################################################################################
if(BIELD_COMPILER_MSVC)
  if(BIELD_COMPILE_WITH_HIGHEST_WARNING_LEVEL)
    # Highest warning level.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()

  # Multi-threaded compilation in all configurations.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  # Treat warnings as errors in all configurations.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
  # Disable RTTI in all configurations.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
  # Use fast floating point model in all configurations.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:fast")

  if(BIELD_ARCHITECTURE_32BIT)
    # Enable SSE2 on 32 bit (incompatible with /fp:except)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
  endif()

  ### Other configurations
  ##############################################################################
  # Minimal rebuild
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")

  if(MSVC11)
    # Enable debugging optimized code (release builds)
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /d2Zi+")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /d2Zi+")
  endif()

  if(MSVC12)
    # Enable debugging optimized code (release builds)
    set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /Zo")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Zo")
  endif()

  # Maximum optimization, auto-inlining, intrinsic functions
  set(CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE} /Ox /Ob2 /Oi")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Ox /Ob2 /Oi")
endif()

### Global linker flags.
################################################################################
if(BIELD_COMPILER_MSVC)

  # Disable incremental linking
  set(BIELD_LINKER_FLAGS_RELEASE        "${BIELD_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO")
  set(BIELD_LINKER_FLAGS_RELWITHDEBINFO "${BIELD_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO")

  # Remove unreferenced data (incompatible with incremental build)
  set(BIELD_LINKER_FLAGS_RELEASE        "${BIELD_LINKER_FLAGS_RELEASE} /OPT:REF")
  set(BIELD_LINKER_FLAGS_RELWITHDEBINFO "${BIELD_LINKER_FLAGS_RELWITHDEBINFO} /OPT:REF")

  # Not sure what it does but ezEngine is using it... (incompatible with incremental build)
  set(BIELD_LINKER_FLAGS_RELEASE        "${BIELD_LINKER_FLAGS_RELEASE} /OPT:ICF")
  set(BIELD_LINKER_FLAGS_RELWITHDEBINFO "${BIELD_LINKER_FLAGS_RELWITHDEBINFO} /OPT:ICF")
endif()
