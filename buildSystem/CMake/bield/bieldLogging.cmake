################################################################################
### Bield logging configuration and logging functions are defined here.      ###
################################################################################

if(BUILD_LOGGING_INCLUDED)
  return()
endif()
set(BUILD_LOGGING_INCLUDED true)

### Global configuration variables
################################################################################
set(BIELD_LOGGING_VERBOSITY 0 CACHE STRING
    "Higher values produce more verbose output.")
set(BIELD_LOGGING_PREFIX "[bield]" CACHE STRING
    "String used to prefix all log messages. Useful to quickly identify out log messages.")
set(BIELD_LOGGING_DISABLED_WARNINGS "" CACHE STRING
    "List of IDs of disabled warnings")
set(BIELD_LOGGING_SHOW_VERBOSITY_OF_MESSAGE OFF CACHE BOOL
    "Whether to show verbosity of a log message or not.")

mark_as_advanced(BIELD_LOGGING_VERBOSITY
                 BIELD_LOGGING_PREFIX
                 BIELD_LOGGING_DISABLED_WARNINGS
                 BIELD_LOGGING_SHOW_VERBOSITY_OF_MESSAGE)

### Warning IDs
################################################################################
set(BIELD_WARNING_ID_NOT_IMPLEMENTED  0)
set(BIELD_WARNING_ID_UNPARSED_ARGS    1)
set(BIELD_WARNING_ID_TODO             2)
set(BIELD_WARNING_ID_COMPILER_VERSION 3)

### Logging functions
################################################################################

function(bield_generate_log_prefix VERBOSITY OUTPUT_VARIABLE)
  if(BIELD_LOGGING_SHOW_VERBOSITY_OF_MESSAGE)
    set(${OUTPUT_VARIABLE} "${VERBOSITY})${BIELD_LOGGING_PREFIX}" PARENT_SCOPE)
  else()
    set(${OUTPUT_VARIABLE} "${BIELD_LOGGING_PREFIX}" PARENT_SCOPE)
  endif()
endfunction()

function(bield_message _VERBOSITY _STATUS _MESSAGE)
  if(NOT _VERBOSITY GREATER BIELD_LOGGING_VERBOSITY)
    bield_generate_log_prefix(${_VERBOSITY} BIELD_LOGGING_PREFIX)
    message(${_STATUS} "${BIELD_LOGGING_PREFIX} ${_MESSAGE}")
  endif()
endfunction(bield_message)

function(bield_log _VERBOSITY _MESSAGE)
  if(NOT _VERBOSITY GREATER BIELD_LOGGING_VERBOSITY)
    bield_generate_log_prefix(${_VERBOSITY} BIELD_LOGGING_PREFIX)
    message(STATUS "${BIELD_LOGGING_PREFIX} ${_MESSAGE}")
  endif()
endfunction(bield_log)
bield_log(0 "Note: log output verbosity for this project is controlled by the (advanced) cache variable BIELD_LOGGING_VERBOSITY")

function(bield_error _MESSAGE)
  message(SEND_ERROR "${BIELD_LOGGING_PREFIX} ${_MESSAGE}")
endfunction(bield_error)

function(bield_fatal _MESSAGE)
  message(FATAL_ERROR "${BIELD_LOGGING_PREFIX} ${_MESSAGE}")
endfunction(bield_fatal)

function(bield_warning _ID _MESSAGE)
  # warnings will only be printed when the global verbosity is 1 or higher
  if(BIELD_LOGGING_VERBOSITY LESS 1)
    return()
  endif()

  list(FIND "${BIELD_LOGGING_DISABLED_WARNINGS}" "${_ID}" WARNING_INDEX)
  if(WARNING_INDEX EQUAL -1) # not found
    message(AUTHOR_WARNING "${BIELD_LOGGING_PREFIX} ${_MESSAGE}")
  endif()
endfunction(bield_warning)

function(bield_not_implemented)
  bield_warning(${BIELD_WARNING_ID_NOT_IMPLEMENTED} "[not implemented] ${ARGN}")
endfunction(bield_not_implemented)

function(bield_warning_unparsed_args)
  bield_warning(${BIELD_WARNING_ID_UNPARSED_ARGS} "[unparsed args] ${ARGN}")
endfunction(bield_warning_unparsed_args)

function(bield_error_unparsed_args)
  bield_error("[unparsed args] ${ARGN}")
endfunction(bield_error_unparsed_args)

function(bield_todo)
  bield_warning(${BIELD_WARNING_ID_TODO} "[todo] ${ARGN}")
endfunction()

function(bield_warning_compiler_version)
  bield_warning(${BIELD_WARNING_ID_COMPILER_VERSION} "[compiler version] ${ARGN}")
endfunction()

function(bield_indent_log_prefix INDENT_STRING)
  set(BIELD_LOGGING_PREFIX "${BIELD_LOGGING_PREFIX}${INDENT_STRING}" PARENT_SCOPE)
endfunction(bield_indent_log_prefix)

function(bield_unindent_log_prefix UNINDENT_AMOUNT)
  string(LENGTH "${BIELD_LOGGING_PREFIX}" LEN)
  math(EXPR LEN "${LEN}-${UNINDENT_AMOUNT}")
  string(SUBSTRING "${BIELD_LOGGING_PREFIX}" 0  LEN RESULT)
  set(BIELD_LOGGING_PREFIX "${RESULT}" PARENT_SCOPE)
endfunction(bield_unindent_log_prefix)
