#pragma once

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  #ifdef KR_ENGINE_DLL_EXPORT
    #define KR_ENGINE_API __declspec(dllexport)
  #else
    #define KR_ENGINE_API __declspec(dllimport)
  #endif
#else
  #define KR_ENGINE_API
#endif

namespace kr
{
  using ::std::swap;
  using ::std::move;
  using ::std::forward;
  using ::std::begin;
  using ::std::end;

  KR_ENGINE_API const char* getSomething();
}

#define KR_NOT_IMPLEMENTED throw std::exception("Not implemented.")

#include <krEngine/common/ezArrayPtrExtension.h>
#include <krEngine/common/ezTypesExtension.h>
