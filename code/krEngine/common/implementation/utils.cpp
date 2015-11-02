#include <krEngine/common/utils.h>

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  #include "utils_Windows.inl"
#else
  #error common/utils not implemented for this platform.
#endif
