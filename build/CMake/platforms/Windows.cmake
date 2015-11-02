# Windows specific settings.

if(MSVC)
  add_compile_options(
    # Enable multi-threaded compilation.
    #"/MP"

    # Treat warnings as errors.
    "/WX"

    # Disable RTTI.
    "/GR-"

    # Use fast floating point model.
    "/fp:fast"

    # Enable floating point exceptions.
    #"/fp:except"

    # Enable debugging local variables in optimized code.
    $<$<CONFIG:RelWithDebInfo>:"/Zo">

    $<$<CONFIG:Release>:"/INCREMENTAL:NO">
    $<$<CONFIG:Release>:"/OPT:REF">
    $<$<CONFIG:Release>:"/OPT:ICF">
  )
else()
  message(WARNING "Unknown compiler on Windows.")
endif()
