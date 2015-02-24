#pragma once
#include <krEngine/rendering/window.h>

namespace kr
{
  namespace Renderer
  {
    KR_ENGINE_API void extract();
    KR_ENGINE_API void update(ezTime dt, RefCountedPtr<Window> pTarget);
  };
}
