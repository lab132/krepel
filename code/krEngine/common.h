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
}

#define KR_NOT_IMPLEMENTED EZ_ASSERT_NOT_IMPLEMENTED

#include <krEngine/common/ezMakeArrayPtr.h>
#include <krEngine/common/ezTypesExtension.h>
#include <krEngine/common/utils.h>
