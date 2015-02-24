#pragma once
#include <krEngine/referenceCounting.h>
#include <krEngine/rendering/texture.h>

#include <CoreUtils/Image/Image.h>

namespace kr
{
  class TextureInstance : public RefCounted
  {
  public:
    using ReleasePolicy = kr::RefCountedReleasePolicies::DoNothing;

  public:
    ezString128 name;
    ezImage image;
  };

  struct TextureInfo
  {
    Texture::Handle handle;
    TextureInstance* pTexture = nullptr;
  };

  bool isValid(TextureInfo& info);

  TextureInfo& findInstance(Texture::Handle hTex);

  TextureInfo& findInstance(const char* textureName);

  /// \brief Fails if \a hTex is not valid.
  TextureInfo& getInstance(kr::Texture::Handle hTex);

  TextureInfo& getFreeInstance();
}
