#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/implementation/textureInstance.h>

#include <Foundation/IO/FileSystem/FileSystem.h>

using TextureContainer = ezDynamicArray<kr::TextureInfo>;

static TextureContainer* g_pTextures;
static kr::TextureInfo g_invalidInfo;

namespace
{
  EZ_BEGIN_SUBSYSTEM_DECLARATION(SpriteEngine, Textures)
    BEGIN_SUBSYSTEM_DEPENDENCIES
      "Foundation",
      "Core"
    END_SUBSYSTEM_DEPENDENCIES

    ON_CORE_STARTUP
    {
      g_pTextures = new (m_mem_textures) TextureContainer();
    }

    ON_CORE_SHUTDOWN
    {
      for (ezUInt32 i = 0; i < g_pTextures->GetCount(); ++i)
      {
        auto& info = (*g_pTextures)[i];
        EZ_ASSERT(info.pTexture == nullptr,
                  "Some textures were NOT properly unlodaded.");
      }

      g_pTextures->~TextureContainer();
      g_pTextures = nullptr;
    }

  private:
    char m_mem_textures[sizeof(TextureContainer)];
  EZ_END_SUBSYSTEM_DECLARATION
}

bool kr::isValid(kr::TextureInfo& info)
{
  return &info != &g_invalidInfo;
}

kr::TextureInfo& kr::findInstance(Texture::Handle hTex)
{
  // Is hTex invalid?
  if (hTex == Texture::Handle()
   && hTex.index < g_pTextures->GetCount())
    return g_invalidInfo;

  return (*g_pTextures)[hTex.index];
}

kr::TextureInfo& kr::findInstance(const char* textureName)
{
  for (ezUInt32 i = 0; i < g_pTextures->GetCount(); ++i)
  {
    auto& info = (*g_pTextures)[i];
    if (info.pTexture && info.pTexture->name == textureName)
      return info;
  }

  return g_invalidInfo;
}

/// \brief Fails if \a hTex is not valid.
kr::TextureInfo& kr::getInstance(Texture::Handle hTex)
{
  EZ_ASSERT(hTex != Texture::Handle(), "Invalid handle.");
  auto& info = (*g_pTextures)[hTex.index];
  EZ_ASSERT(info.handle == hTex, "Invalid handle.");
  EZ_ASSERT(info.pTexture != nullptr, "Handle to freed texture.");
  return info;
}

kr::TextureInfo& kr::getFreeInstance()
{
  for (ezUInt32 i = 0; i < g_pTextures->GetCount(); ++i)
  {
    auto& info = (*g_pTextures)[i];
    if (info.pTexture == nullptr)
      return info;
  }

  auto& info = g_pTextures->ExpandAndGetRef();
  info.handle.hash = 0;
  info.handle.index = g_pTextures->GetCount() - 1;
  return info;
}

kr::Texture::Handle kr::Texture::load(const char* filename)
{
  EZ_LOG_BLOCK("Loading Texture", filename);

  // See if this texture is already loaded.
  {
    auto& info = findInstance(filename);
    if(isValid(info) && info.pTexture != nullptr)
    {
      addRef(info.pTexture);
      return info.handle;
    }
  }

  // At this point, we know the texture is not loaded, so we load it now.
  ezImage img;
  if (img.LoadFrom(filename).Failed())
  {
    // Loading failed => return an invalid handle.
    return Handle();
  }

  auto& info = getFreeInstance();
  EZ_ASSERT(isValid(info) && info.pTexture == nullptr, "Forgot to call delete somewhere?");
  info.pTexture = EZ_DEFAULT_NEW(TextureInstance);
  addRef(info.pTexture);
  info.pTexture->name = filename;
  info.pTexture->image = move(img);

  return info.handle;
}

void kr::Texture::unload(Handle& hTex)
{
  auto& info = getInstance(hTex);

  EZ_LOG_BLOCK("Unloading Texture", info.pTexture->name.GetData());

  {
    auto pTex = info.pTexture;
    releaseRef(pTex);
  }

  if (!isReferenced(info.pTexture))
  {
    info.handle.hash = info.handle.hash < 255 ?
                       info.handle.hash + 1 :
                       0;
    EZ_DEFAULT_DELETE(info.pTexture);
  }

  // Invalidate handle.
  hTex = Handle();
}

const ezString64& kr::Texture::getName(Handle hTex)
{
  // TODO Implement me.
  throw std::exception("Not implemented.");
}

const ezImage& kr::Texture::getImage(Handle hTex)
{
  // TODO Implement me.
  throw std::exception("Not implemented.");
}

ezUInt32 kr::Texture::getWidth(Handle hTex)
{
  return getInstance(hTex).pTexture->image.GetWidth();
}

ezUInt32 kr::Texture::getHeight(Handle hTex)
{
  return getInstance(hTex).pTexture->image.GetHeight();
}

bool kr::Texture::Handle::isValid() const
{
  // First, check we are equal to the the generally invalid handle.
  if (*this == Texture::Handle())
    return false;

  // Then, get the info we are referencing. We're a handle afterall.
  auto& info = findInstance(*this);

  // Compare if the info handle is equal to hTex and that
  // info does not contain a freed texture instance.
  return kr::isValid(info) && info.handle == *this && info.pTexture != nullptr;
}
