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
  using std::swap;

  KR_ENGINE_API const char* getSomething();
}

#define KR_NOT_IMPLEMENTED EZ_REPORT_FAILURE("Not implemented.")
