### Include guard
################################################################################
if(BIELD_CONFIG_LINUX_INCLUDED)
  return()
endif()
set(BIELD_CONFIG_LINUX_INCLUDED ON)

### Set defaults. These may be overridden later within this script.
################################################################################
# Running on Linux.
set(BIELD_PLATFORM_LINUX ON)

# Running on POSIX.
set(BIELD_PLATFORM_POSIX ON)

# Using make?
set(BIELD_GENERATOR_MAKE OFF)

# Compiling for a 32 bit architecture?
set(BIELD_ARCHITECTURE_32BIT OFF)

# Compiling for a 64 bit architecture?
set(BIELD_ARCHITECTURE_64BIT OFF)

# String that describes the used compiler.
set(BIELD_COMPILER_STRING "-NOTFOUND")

# String that describes the platform architecture (32/64 bit).
set(BIELD_ARCHITECTURE_STRING "-NOTFOUND")

# String that describes the generator used (e.g. "Make").
set(BIELD_GENERATOR_STRING "-NOTFOUND")

# Compile with highest warning level by default on Linux.
set(BIELD_COMPILE_WITH_HIGHEST_WARNING_LEVEL ON)

# Info that will be logged once all information is gathered.
set(INFO "Platform: Linux")

### Detect architecture.
################################################################################
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(BIELD_ARCHITECTURE_64BIT ON)
  set(BIELD_ARCHITECTURE_STRING "64")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(BIELD_ARCHITECTURE_32BIT ON)
  set(BIELD_ARCHITECTURE_STRING "32")
else()
  bield_fatal("Enable to determine size of void* on Linux!")
endif()

set(INFO "${INFO} ${BIELD_ARCHITECTURE_STRING} bit")

### Detect generator.
################################################################################
if(CMAKE_GENERATOR STREQUAL "Unix Makefiles")
  set(BIELD_GENERATOR_MAKE ON)
  set(BIELD_GENERATOR_STRING "Make")
else()
  bield_fatal("Generator '${CMAKE_GENERATOR}' is not supported on Linux!")
endif()

set(INFO "${INFO} using ${BIELD_GENERATOR_STRING}")

### Detect compiler.
################################################################################
if(CMAKE_CXX_COMPILER)
  set(BIELD_COMPILER_STRING "${CMAKE_CXX_COMPILER}")
elseif("$ENV{CXX}")
  set(BIELD_COMPILER_STRING "$ENV{CXX}")
else()
  set(BIELD_COMPILER_STRING "<auto>")
endif()

# If the BIELD_COMPILER_STRING is empty, set it to <auto>, indicating that the
# system's default compiler is used.
if(NOT BIELD_COMPILER_STRING)
  set(BIELD_COMPILER_STRING "<auto>")
endif()

set(INFO "${INFO} and ${BIELD_COMPILER_STRING}")

### Process collected data.
################################################################################
bield_log(1 "${INFO}.")

# Depends on the _STRING variables.
bield_set_output_directories("Linux")

### Compiler flags.
################################################################################
if(BIELD_COMPILE_WITH_HIGHEST_WARNING_LEVEL)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# Treat warnings as errors.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

# dynamic linking will fail without fPIC (plugins)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Wno-enum-compare removes all annoying enum cast warnings.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-enum-compare")

# std=c++11 is - well needed for c++11.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Enable SSE.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mssse3")

# Use SSE for floating point math.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpmath=sse")

# gdwarf-3 will use the old debug info
# which is compatible with older gdb versions.
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-3")

# Flags for C projects?
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -gdwarf-3")

### Linker flags.
################################################################################

# TODO Add some default linker flags?
