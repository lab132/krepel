#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/implementation/textureImpl.h>

#include <Foundation/IO/FileSystem/FileSystem.h>

namespace{ enum { MaxTextureSlots = 80 };}

using TextureContainer = ezDynamicArray<kr::TextureImpl*>;
using TextureSlots = ezStaticArray<GLuint, MaxTextureSlots>;

static TextureContainer* g_pTextures;
static TextureSlots* g_pFreeTextureSlots;
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
      g_pFreeTextureSlots = new (m_mem_textureSlots) TextureSlots();

      for (GLuint i = GL_TEXTURE0; i < GL_TEXTURE0 + MaxTextureSlots; ++i)
      {
        g_pFreeTextureSlots->PushBack(i);
      }

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
    char m_mem_textureSlots[sizeof(TextureSlots)];
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

GLuint kr::getNextFreeSlot()
{
  EZ_ASSERT(!g_pFreeTextureSlots->IsEmpty(),
            "No more available textures slots. Max: %u", MaxTextureSlots);
  auto slot = g_pFreeTextureSlots->PeekBack();
  g_pFreeTextureSlots->PopBack();
  return slot;
}

void kr::freeSlot(GLuint slot)
{
  g_pFreeTextureSlots->PushBack(slot);
}


kr::TextureImpl::~TextureImpl()
{
  freeSlot(m_slot);
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
  pTex->m_slot = getNextFreeSlot();

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

ezUInt32 kr::Texture::getSlot() const
{
  return static_cast<ezUInt32>(getImpl(this)->m_slot);
}
