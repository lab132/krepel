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
    KR_ENGINE_API ezUInt32 getId() const;

  protected: // *** Construction
    Texture() = default; ///< Default ctor (private).

  private: // *** Type Constraints
    Texture(const Texture&) = delete;         ///< No copy.
    void operator =(const Texture&) = delete; ///< No assignment.
  };

  struct TextureSlot
  {
    ezInt32 value = -1;

    TextureSlot() = default;
    explicit TextureSlot(ezInt32 v) : value(v){}
  };

  /// \brief Restores the texture 2D binding at the end of the scope.
  struct RestoreTexture2dOnScopeExit
  {
    ezInt32 previous;

    KR_ENGINE_API RestoreTexture2dOnScopeExit();
    KR_ENGINE_API ~RestoreTexture2dOnScopeExit();
  };

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

  public: // *** Construction
    KR_ENGINE_API ~Sampler();

  protected: // *** Construction
    Sampler() = default; ///< Protected default ctor.

  private: // *** Construction
    Sampler(const Sampler&) = delete;         ///< No copy.
    void operator =(const Sampler&) = delete; ///< No assignment.
  };

  using SamplerPtr = RefCountedPtr<Sampler>;

  KR_ENGINE_API void use(SamplerPtr pSampler, TextureSlot slot);
  KR_ENGINE_API void unuse(SamplerPtr pSampler, TextureSlot slot);
}
