#pragma once
#include <krEngine/ownership.h>
#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/vertexBuffer.h>
#include <krEngine/rendering/shader.h>

#include <Foundation/Reflection/Reflection.h>

/// \note Ez's reflection requires the type to live in the top-level, not in any namespace.
struct krSpriteVertex
{
  // *** Per-Vertex Data
  ezVec2 pos = ezVec2::ZeroVector();
  ezVec2 texCoords = ezVec2::ZeroVector();
};

EZ_DECLARE_REFLECTABLE_TYPE(EZ_NO_LINKAGE, krSpriteVertex);

namespace kr
{
  namespace Renderer{ class Extractor; };

  EZ_DECLARE_FLAGS(ezUInt8, SpriteComponents,
                   ShaderUniforms,
                   Cutout,
                   LocalBounds);

  class KR_ENGINE_API Sprite
  {
  public: // *** Util
    static Owned<ShaderProgram> createDefaultShader();

  public: // *** Construction
    Sprite();

  public: // *** Accessors/Mutators
    bool needsUpdate() const { return m_needUpdate.GetValue() != 0; }

    void setColor(ezColor c) { m_color = move(c); }
    ezColor getColor() const { return m_color; }

    /// \name Texture
    /// \{

    /// \brief Set the current texture.
    void setTexture(Borrowed<Texture> pTex)
    {
      m_pTexture = pTex;
      m_needUpdate.Add(SpriteComponents::Cutout);
    }

    /// \brief Gets the handle to the current texture.
    Borrowed<Texture> getTexture() { return m_pTexture; }

    Borrowed<const Texture> getTexture() const { return m_pTexture; }

    /// \}

    /// \name Sampler
    /// \{

    void setSampler(Borrowed<Sampler> sampler) { this->m_pSampler = move(sampler); }

    Borrowed<Sampler> getSampler() { return this->m_pSampler; }

    Borrowed<const Sampler> getSampler() const { return this->m_pSampler; }

    /// \}

    /// \name Shader Program
    /// \{

    void setShader(Borrowed<ShaderProgram> shader)
    {
      this->m_pShader = move(shader);
      this->m_needUpdate.Add(SpriteComponents::ShaderUniforms);
    }

    Borrowed<ShaderProgram> getShader() { return m_pShader; }

    Borrowed<const ShaderProgram> getShader() const { return m_pShader; }

    /// \}

    /// \brief Dimensions of the sprite.
    void setLocalBounds(ezRectFloat newLocalBounds);

    /// \brief Get the bounds of this sprite.
    ezRectFloat getLocalBounds() const { return m_localBounds; }

    void setCutout(ezRectU32 newCutout);

    ShaderUniform getTextureUniform() const { return m_uTexture; }
    ShaderUniform getColorUniform() const { return m_uColor; }
    ShaderUniform getOriginUniform() const { return m_uOrigin; }
    ShaderUniform getRotationUniform() const { return m_uRotation; }
    ShaderUniform getViewMatrixUniform() const { return m_uViewMatrix; }
    ShaderUniform getProjectionMatrixUniform() const { return m_uProjectionMatrix; }

    Borrowed<VertexBuffer> getVertexBuffer() { return this->m_pVertexBuffer; }
    Borrowed<const VertexBuffer> getVertexBuffer() const { return this->m_pVertexBuffer; }

    ezArrayPtr<const krSpriteVertex> getVertices() const { return ezMakeArrayPtr(m_vertices); }

  public: // *** Friends
    friend KR_ENGINE_API void update(Sprite& sprite);

  private: // *** Data
    /// \brief 1's for all components that need updating.
    ezBitflags<SpriteComponents> m_needUpdate;

    /// \brief The quad the texture will be rendered to.
    krSpriteVertex m_vertices[4];

    Owned<VertexBuffer> m_pVertexBuffer;

    Borrowed<Sampler> m_pSampler;

    /// \brief Handle to the texture used by this sprite.
    Borrowed<Texture> m_pTexture;

    /// \brief Local bounds of this sprite.
    ezRectFloat m_localBounds = { 0.0f, 0.0f };

    /// \brief Cutout of the texture. By default, the entire texture is shown.
    ezRectU32 m_cutout = { 0u, 0u };

    ezColor m_color = ezColor::White;

    Borrowed<ShaderProgram> m_pShader;

    ShaderUniform m_uTexture;
    ShaderUniform m_uColor;
    ShaderUniform m_uTransform;
    ShaderUniform m_uOrigin;
    ShaderUniform m_uRotation;
    ShaderUniform m_uViewMatrix;
    ShaderUniform m_uProjectionMatrix;
  };

  /// \brief Whether the given \a sprite is ready to render.
  ///
  /// Use this for validation before extracting sprite data.
  KR_ENGINE_API bool canRender(const Sprite& sprite);

  /// \brief Updates the internal state of \a sprite.
  KR_ENGINE_API void update(Sprite& sprite);

  /// \brief Initializes a sprite.
  /// \note This is actually just an update call.
  inline ezResult initialize(Sprite& sprite,
                             Borrowed<Texture> texture,
                             Borrowed<Sampler> sampler,
                             Borrowed<ShaderProgram> shader)
  {
    sprite.setTexture(texture);
    sprite.setSampler(sampler);
    sprite.setShader(shader);
    update(sprite);
    bool success = canRender(sprite) && !sprite.needsUpdate();
    return success ? EZ_FAILURE : EZ_SUCCESS;
  }
}
