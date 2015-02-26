#pragma once
#include <krEngine/rendering/sprite.h>
#include <krEngine/transform2D.h>

namespace kr
{
  KR_ENGINE_API void extract(Renderer::Extractor& e,
                             const Sprite& sprite,
                             Transform2D transform);
}
