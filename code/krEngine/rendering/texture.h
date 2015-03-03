#pragma once
#include <krEngine/referenceCounting.h>

#include <CoreUtils/Image/Image.h>

namespace kr
{
  using TextureName = ezString128;

  class Texture : public RefCounted
  {
  public:
    using ReleasePolicy = Texture;

    KR_ENGINE_API static void release(Texture*& pTex);

    /// \brief Loads a texture from the filesystem with the given \a filename.
    KR_ENGINE_API static RefCountedPtr<Texture> load(const char* filename);

  public: // *** Accessors/Mutators

    /// \brief Get the underlying image data of the texture.
    KR_ENGINE_API const ezImage& getImage() const;

    /// \brief Gets the name of the texture.
    ///
    /// If the texture was loaded from a file, this will be the filename.
    /// Else the name will be the given string.
    KR_ENGINE_API const TextureName& getName() const;

    ezUInt32 getWidth() const { return getImage().GetWidth(); }
    ezUInt32 getHeight() const { return getImage().GetHeight(); }

    /// The result of glGenTextures()
    KR_ENGINE_API ezUInt32 getGlHandle() const;

  protected: // *** Construction
    Texture() = default; ///< Default ctor (private).

  private: // *** Type Constraints
    Texture(const Texture&) = delete;         ///< No copy.
    void operator =(const Texture&) = delete; ///< No assignment.
  };

  using TexturePtr = RefCountedPtr<Texture>;

  struct TextureSlot
  {
    ezInt32 value = -1;

    TextureSlot() = default;
    explicit TextureSlot(ezInt32 v) : value(v){}
  };

  KR_ENGINE_API ezResult bind(TexturePtr pTexture, TextureSlot slot);
  KR_ENGINE_API ezResult restoreLastTexture2D(TextureSlot slot);

  // Texture Sampling
  // ================

  enum class TextureFiltering
  {
    Nearest,
    Linear,
    NearestMipMapNearest,
    LinearMipMapNearest,
    NearestMpMapLinear,
    LinearMpMapLinear,
  };

  enum class TextureWrapping
  {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
  };

  class Sampler : public RefCounted
  {
  public: // *** Static API
    using ReleasePolicy = RefCountedReleasePolicies::DefaultDelete;

    KR_ENGINE_API static RefCountedPtr<Sampler> create();

  public: // *** Data
    ezUInt32 m_glHandle;

  private: // *** Data
    TextureFiltering m_filtering = TextureFiltering::Nearest;
    TextureWrapping m_wrapping = TextureWrapping::Repeat;

  public: // *** Accessors/Mutators
    KR_ENGINE_API void setFiltering(TextureFiltering filtering);
    TextureFiltering getFiltering() const { return m_filtering; }

    KR_ENGINE_API void setWrapping(TextureWrapping wrapping);
    TextureWrapping getWrapping() const { return m_wrapping; }

    ezUInt32 getGlHandle() const { return m_glHandle; }

  public: // *** Construction
    KR_ENGINE_API ~Sampler();

  protected: // *** Construction
    Sampler() = default; ///< Protected default ctor.

  private: // *** Construction
    Sampler(const Sampler&) = delete;         ///< No copy.
    void operator =(const Sampler&) = delete; ///< No assignment.
  };

  using SamplerPtr = RefCountedPtr<Sampler>;

  KR_ENGINE_API ezResult bind(SamplerPtr pSampler, TextureSlot slot);

  KR_ENGINE_API ezResult restoreLastSampler(TextureSlot slot);
}

#define KR_RAII_BIND_TEXTURE_2D(tex, slot) \
  ::kr::bind(tex, slot);                   \
  KR_ON_SCOPE_EXIT { ::kr::restoreLastTexture2D(slot); }

#define KR_RAII_BIND_SAMPLER(sampler, slot) \
  ::kr::bind(sampler, slot);                   \
  KR_ON_SCOPE_EXIT { ::kr::restoreLastSampler(slot); }
