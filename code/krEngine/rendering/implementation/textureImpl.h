#pragma once
#include <krEngine/referenceCounting.h>
#include <krEngine/rendering/texture.h>

namespace kr
{
  class TextureImpl : public Texture
  {
  public: // *** Data
    GLuint m_glHandle = 0;
    TextureName m_name;
    ezImage m_image;
  };

  TextureImpl* getImpl(Texture* pTex);
  const TextureImpl* getImpl(const Texture* pTex);
}
