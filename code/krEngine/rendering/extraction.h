#pragma once
#include <krEngine/rendering/sprite.h>

namespace kr
{
  KR_ENGINE_API void extract(Renderer::Extractor& e,
                             const Sprite& sprite,
                             ezTransform transform);
}
