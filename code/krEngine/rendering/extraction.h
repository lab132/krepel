#pragma once
#include <krEngine/rendering/sprite.h>
#include <krEngine/transform2D.h>

class ezCamera;

namespace kr
{
  KR_ENGINE_API void extract(Renderer::Extractor& e,
                             const ezCamera& cam,
                             float aspectRatio);

  KR_ENGINE_API void extract(Renderer::Extractor& e,
                             const Sprite& sprite,
                             Transform2D transform);
}
