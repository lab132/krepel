################################################################################
### This script basically just includes all dependencies to make Bield       ###
### work properly. It also guards against multiple inclusions during the     ###
### same CMake generation phase.                                             ###
################################################################################

# Make sure users include this script after their first call to project(...)
if(NOT CMAKE_SYSTEM_NAME)
  message(SEND_ERROR "You have to call include(bield) AFTER the first call to project(...).")
endif()

### Include guard
################################################################################
if(BIELD_INCLUDED)
  return()
endif()
set(BIELD_INCLUDED ON)

################################################################################
### Dependencies ###############################################################
################################################################################

# Used for easier parsing of optional arguments passed to bield_ functions.
# Shipped with CMake itself.
include(CMakeParseArguments)

# Used to print all properties of a target.
# Should be located right next to this script (bield.cmake).
include(echoTargetProperties)

include(cotire)

################################################################################
### Bield scripts ##############################################################
################################################################################

# Bield script that defines all logging-related stuff.
# Most importantly the function bield_log(123 ""), which should be used
# instead of message(STATUS "").
include(bield/bieldLogging)

# All bield_ utility functions.
include(bield/bieldUtilities)

# Compile and link flags settings and utilities.
include(bield/bieldFlags)

# General configuration.
include(bield/bieldGeneralConfig)

### Platform specific configurations.
################################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  include(bield/bieldConfig_Windows)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin" AND CURRENT_OSX_VERSION)
  include(bield/bieldConfig_OSX)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  include(bield/bieldConfig_Linux)
else()
  bield_fatal("Unsupported platform '${CMAKE_SYSTEM_NAME}'.")
endif()
