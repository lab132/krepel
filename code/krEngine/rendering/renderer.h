#pragma once
#include <krEngine/rendering/window.h>

/// \todo remove this
#include <krEngine/rendering/sprite.h>
namespace kr
{
  namespace Renderer
  {
    KR_ENGINE_API void extract();
    KR_ENGINE_API void update(ezTime dt, RefCountedPtr<Window> pTarget);
    KR_ENGINE_API void update(ezTime dt, RefCountedPtr<Window> pTarget, Sprite& sprite);
  };
}
