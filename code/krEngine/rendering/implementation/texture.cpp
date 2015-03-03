#include <krEngine/pch.h>
#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/implementation/textureImpl.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#include <Foundation/IO/FileSystem/FileSystem.h>

using TextureContainer = ezDynamicArray<kr::TextureImpl*>;
using TextureBindings = ezHybridArray<kr::TexturePtr, 8>;
using SamplerBindings = ezHybridArray<kr::SamplerPtr, 8>;

static TextureContainer* g_pTextures;
static TextureBindings* g_pTextureBindings;
static SamplerBindings* g_pSamplerBindings;
static bool g_initialized = false;

EZ_BEGIN_SUBSYSTEM_DECLARATION(krEngine, Textures)
  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORE_STARTUP
  {
    g_pTextures = new (m_mem_textures) TextureContainer();
    g_pTextureBindings = new (m_mem_textureBindings) TextureBindings();
    g_pSamplerBindings = new (m_mem_samplerBindings) SamplerBindings();

    g_initialized = true;
  }

  ON_CORE_SHUTDOWN
  {
    // Sampler Bindings
    // ================
    if (g_pSamplerBindings->GetCount() > 0)
    {
      ezLog::Error("There are still %u active sampler bindings!",
                    g_pSamplerBindings->GetCount());
    }

    g_pSamplerBindings->~SamplerBindings();
    g_pSamplerBindings = nullptr;

    // Texture Bindings
    // ================
    if (g_pTextureBindings->GetCount() > 0)
    {
      ezLog::Error("There are still %u active texture bindings!",
                    g_pTextureBindings->GetCount());
    }

    g_pTextureBindings->~TextureBindings();
    g_pTextureBindings = nullptr;

    // Texture Instances
    // =================
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
  ezUByte m_mem_textures[sizeof(TextureContainer)];
  ezUByte m_mem_textureBindings[sizeof(TextureBindings)];
  ezUByte m_mem_samplerBindings[sizeof(SamplerBindings)];
EZ_END_SUBSYSTEM_DECLARATION

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
  glCheck(glGenTextures(1, &pTex->m_glHandle));

  // Upload Pixel Data
  // =================
  auto pixels = pTex->m_image.GetSubImagePointer<void>();

  KR_RAII_BIND_TEXTURE_2D(pTex, TextureSlot(0));

  glCheck(glBindTexture(GL_TEXTURE_2D, pTex->m_glHandle));
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

ezUInt32 kr::Texture::getGlHandle() const
{
  return static_cast<ezUInt32>(getImpl(this)->m_glHandle);
}

ezResult kr::bind(TexturePtr pTexture, TextureSlot slot)
{
  if (isNull(pTexture))
  {
    ezLog::Warning("Cannot bind nullptr as texture. Ignoring.");
    return EZ_FAILURE;
  }

  auto handle = pTexture->getGlHandle();

  // Set the active texture unit.
  glCheck(glActiveTexture(GL_TEXTURE0 + slot.value));

  // Bind the texture.
  glCheck(glBindTexture(GL_TEXTURE_2D, handle));

  // Save the texture ptr.
  g_pTextureBindings->ExpandAndGetRef() = move(pTexture);

  return EZ_SUCCESS;
}

ezResult kr::restoreLastTexture2D(TextureSlot slot)
{
  if (g_pTextureBindings->IsEmpty())
  {
    ezLog::Warning("No last texture 2D to restore!");
    return EZ_FAILURE;
  }

  // Drop the current binding.
  g_pTextureBindings->PopBack();

  // Set the active texture unit.
  glCheck(glActiveTexture(GL_TEXTURE0 + slot.value));

  if(g_pTextureBindings->IsEmpty())
  {
    glCheck(glBindTexture(GL_TEXTURE_2D, 0));
    return EZ_SUCCESS;
  }

  // Get the handle of the current binding.
  auto handle = g_pTextureBindings->PeekBack()->getGlHandle();

  // And actually bind it again.
  glCheck(glBindTexture(GL_TEXTURE_2D, handle));

  return EZ_SUCCESS;
}

// static
kr::RefCountedPtr<kr::Sampler> kr::Sampler::create()
{
  GLuint h;
  glCheck(glGenSamplers(1, &h));
  if (glIsSampler(h) != GL_TRUE)
  {
    ezLog::Warning("Unable to create sampler object.");
    return nullptr;
  }

  auto pSampler = EZ_DEFAULT_NEW(Sampler);
  pSampler->m_glHandle = h;
  pSampler->setFiltering(pSampler->m_filtering);
  pSampler->setWrapping(pSampler->m_wrapping);
  return pSampler;
}

kr::Sampler::~Sampler()
{
  glCheck(glDeleteSamplers(1, &m_glHandle));
}

static GLenum translate(kr::TextureFiltering f)
{
  switch(f)
  {
  case kr::TextureFiltering::Nearest:              return GL_NEAREST;
  case kr::TextureFiltering::Linear:               return GL_LINEAR;
  case kr::TextureFiltering::NearestMipMapNearest: return GL_NEAREST_MIPMAP_NEAREST;
  case kr::TextureFiltering::LinearMipMapNearest:  return GL_LINEAR_MIPMAP_NEAREST;
  case kr::TextureFiltering::NearestMpMapLinear:   return GL_NEAREST_MIPMAP_LINEAR;
  case kr::TextureFiltering::LinearMpMapLinear:    return GL_LINEAR_MIPMAP_LINEAR;
  default:
    break;
  }

  EZ_REPORT_FAILURE("Invalid input.");
  return 0;
}

static GLenum translate(kr::TextureWrapping w)
{
  switch(w)
  {
  case kr::TextureWrapping::Repeat:         return GL_REPEAT;
  case kr::TextureWrapping::MirroredRepeat: return GL_MIRRORED_REPEAT;
  case kr::TextureWrapping::ClampToEdge:    return GL_CLAMP_TO_EDGE;
  case kr::TextureWrapping::ClampToBorder:  return GL_CLAMP_TO_BORDER;
  default:
    break;
  }

  EZ_REPORT_FAILURE("Invalid input.");
  return 0;
}

void kr::Sampler::setFiltering(TextureFiltering filtering)
{
  m_filtering = filtering;
  auto value = translate(filtering);
  glCheck(glSamplerParameteri(m_glHandle, GL_TEXTURE_MIN_FILTER, value));
  glCheck(glSamplerParameteri(m_glHandle, GL_TEXTURE_MAG_FILTER, value));
}

void kr::Sampler::setWrapping(TextureWrapping wrapping)
{
  m_wrapping = wrapping;
  auto value = translate(m_wrapping);
  glCheck(glSamplerParameteri(m_glHandle, GL_TEXTURE_WRAP_S, value));
  glCheck(glSamplerParameteri(m_glHandle, GL_TEXTURE_WRAP_T, value));
}

ezResult kr::bind(SamplerPtr pSampler, TextureSlot slot)
{
  if (isNull(pSampler))
  {
    ezLog::Warning("Cannot bind nullptr as sampler. Ignoring.");
    return EZ_FAILURE;
  }

  auto handle = pSampler->getGlHandle();

  // Bind the texture.
  glCheck(glBindSampler(slot.value, pSampler->m_glHandle));

  // Save the handle.
  g_pSamplerBindings->ExpandAndGetRef() = move(pSampler);

  return EZ_SUCCESS;
}

ezResult kr::restoreLastSampler(TextureSlot slot)
{
  if (g_pSamplerBindings->IsEmpty())
  {
    ezLog::Warning("No last texture 2D to restore!");
    return EZ_FAILURE;
  }

  // Drop the current binding.
  g_pSamplerBindings->PopBack();

  if(g_pSamplerBindings->IsEmpty())
  {
    glCheck(glBindSampler(slot.value, 0));
    return EZ_SUCCESS;
  }

  // Get the handle of the current binding.
  auto handle = g_pSamplerBindings->PeekBack()->getGlHandle();

  // And actually bind it again.
  glCheck(glBindSampler(slot.value, handle));

  return EZ_SUCCESS;
}
