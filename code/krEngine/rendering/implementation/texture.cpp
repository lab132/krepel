#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/implementation/textureImpl.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#include <Foundation/IO/FileSystem/FileSystem.h>

using TextureContainer = ezDynamicArray<kr::TextureImpl*>;

static TextureContainer* g_pTextures;
static bool g_initialized = false;

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

      g_initialized = true;
    }

    ON_CORE_SHUTDOWN
    {
      if (g_pTextures->GetCount() > 0)
      {
        ezLog::Error("%u Texture(s) were not properly released yet!",
                     g_pTextures->GetCount());
      }

      g_pTextures->~TextureContainer();
      g_pTextures = nullptr;

      g_initialized = false;
    }

  private:
    char m_mem_textures[sizeof(TextureContainer)];
  EZ_END_SUBSYSTEM_DECLARATION
}

kr::TextureImpl* kr::getImpl(Texture* pTex)
{
  return static_cast<TextureImpl*>(pTex);
}

const kr::TextureImpl* kr::getImpl(const Texture* pTex)
{
  return static_cast<const TextureImpl*>(pTex);
}

void kr::addInstance(TextureImpl* pTex)
{
  g_pTextures->PushBack(pTex);
}

ezResult kr::removeInstance(TextureImpl* pTex)
{
  return g_pTextures->RemoveSwap(pTex) ? EZ_SUCCESS : EZ_FAILURE;
}

kr::TextureImpl* kr::findInstance(const char* textureName)
{
  for (auto pTex : *g_pTextures)
  {
    EZ_ASSERT(pTex, "TextureContainer should not contain any nullptrs!");
    if (pTex->m_name == textureName)
      return pTex;
  }

  return nullptr;
}


kr::TextureImpl::~TextureImpl()
{
  removeInstance(this);
}

// static
void kr::Texture::release(Texture*& pTex)
{
  if (isNull(pTex))
    return;

  auto pImpl = getImpl(pTex);
  EZ_DEFAULT_DELETE(pImpl);
  pTex = nullptr;
}

// static
kr::RefCountedPtr<kr::Texture> kr::Texture::load(const char* filename)
{
  EZ_LOG_BLOCK("Loading Texture", filename);

  EZ_ASSERT(g_initialized, "Textures subsystem not initialized. "
                           "Did you forget to start the ezEngine?");

  // See if this texture is already loaded.
  auto pTex = findInstance(filename);
  if (isValid(pTex))
  {
    ezLog::Dev("Re-using already loaded texture '%s'.", filename);
    return pTex;
  }

  // At this point, we know the texture is not loaded, so we load it now.
  ezImage img;
  if (img.LoadFrom(filename).Failed())
  {
    // Loading failed.
    return nullptr;
  }

  pTex = EZ_DEFAULT_NEW(TextureImpl);
  EZ_ASSERT(isValid(pTex), "Out of memory?");
  pTex->m_name = filename;
  pTex->m_image = move(img);
  glCheck(glGenTextures(1, &pTex->m_id));

  // Upload Pixel Data
  // =================
  auto pixels = pTex->m_image.GetSubImagePointer<void>();
  RestoreTexture2dOnScopeExit restore;
  glCheck(glBindTexture(GL_TEXTURE_2D, pTex->m_id));
  /// \todo Check if GetNumMipLevels() - 1 is ok here.
  glCheck(glTexImage2D(GL_TEXTURE_2D,                       // Target
                       pTex->m_image.GetNumMipLevels() - 1, // (Mip) Level
                       GL_RGBA,                             // Format used by OpenGL (not our pixels).
                       pTex->m_image.GetWidth(),            // Texture width
                       pTex->m_image.GetHeight(),           // Texture height
                       0,                                   // Must be 0, as per the specification.
                       GL_BGRA,                             // Format of the pixels (see last argument).
                       GL_UNSIGNED_BYTE,                    // Size per pixel.
                       pixels));                            // The actual pixel data.

  // Set Default Sampling Options
  // ============================
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
  glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

  addInstance(pTex);

  return pTex;
}

const ezImage& kr::Texture::getImage() const
{
  return getImpl(this)->m_image;
}
const kr::TextureName& kr::Texture::getName() const
{
  return getImpl(this)->m_name;
}

ezUInt32 kr::Texture::getId() const
{
  return static_cast<ezUInt32>(getImpl(this)->m_id);
}

kr::RestoreTexture2dOnScopeExit::RestoreTexture2dOnScopeExit()
{
  glCheck(glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous));
}

kr::RestoreTexture2dOnScopeExit::~RestoreTexture2dOnScopeExit()
{
  glCheck(glBindTexture(GL_TEXTURE_2D, previous));
}
