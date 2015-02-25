#pragma once
#include <krEngine/referenceCounting.h>
#include <krEngine/rendering/texture.h>

namespace kr
{
  class TextureImpl : public Texture
  {
  public: // *** Construction
    ~TextureImpl();

  public: // *** Data
    GLuint m_id = 0;
    TextureName m_name;
    ezImage m_image;
  };

  TextureImpl* getImpl(Texture* pTex);
  const TextureImpl* getImpl(const Texture* pTex);

  void addInstance(TextureImpl* pTex);

  ezResult removeInstance(TextureImpl* pTex);

  /// \return Pointer to a valid texture instance.
  ///         If not found, will yield a \a nullptr.
  TextureImpl* findInstance(const char* textureName);
}
