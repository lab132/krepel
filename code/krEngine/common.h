#pragma once

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  #ifdef krEngine_EXPORTS
    #define KR_ENGINE_API __declspec(dllexport)
  #else
    #define KR_ENGINE_API __declspec(dllimport)
  #endif
#else
  #define KR_ENGINE_API
#endif

namespace kr
{
  KR_ENGINE_API const char* getSomething();
}
