#pragma once
#include <krEngine/referenceCounting.h>
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
                   Cutout,
                   LocalBounds);

  class KR_ENGINE_API Sprite
  {
  public: // *** Construction
    Sprite();

  public: // *** Accessors/Mutators
    bool getNeedsUpdate() const { return m_needUpdate.GetValue() != 0; }

    void setColor(ezColor c) { m_color = move(c); }
    ezColor getColor() const { return m_color; }

    /// \brief Set the current texture.
    void setTexture(TexturePtr pTex)
    {
      m_pTexture = pTex;
      m_needUpdate.Add(SpriteComponents::Cutout);
    }

    /// \brief Gets the handle to the current texture.
    TexturePtr getTexture() const { return m_pTexture; }

    /// \brief Dimensions of the sprite.
    void setLocalBounds(ezRectFloat newLocalBounds);

    /// \brief Get the bounds of this sprite.
    ezRectFloat getLocalBounds() const { return m_localBounds; }

    void setCutout(ezRectU32 newCutout);

    ShaderUniform getTextureUniform() const { return m_uTexture; }
    ShaderUniform getColorUniform() const { return m_uColor; }
    ShaderProgramPtr getShader() const { return m_pShader; }
    VertexBufferPtr getVertexBuffer() const { return m_pVertexBuffer; }
    SamplerPtr getSampler() const { return m_pSampler; }
    ezArrayPtr<const krSpriteVertex> getVertices() const { return ezMakeArrayPtr(m_vertices); }

  public: // *** Friends
    friend KR_ENGINE_API void update(Sprite& sprite);

  private: // *** Data
    /// \brief 1's for all components that need updating.
    ezBitflags<SpriteComponents> m_needUpdate;

    /// \brief The quad the texture will be rendered to.
    krSpriteVertex m_vertices[4];

    ShaderUniform m_uTexture;
    ShaderUniform m_uColor;

    ShaderProgramPtr m_pShader;

    VertexBufferPtr m_pVertexBuffer;

    SamplerPtr m_pSampler;

    /// \brief Handle to the texture used by this sprite.
    TexturePtr m_pTexture;

    /// \brief Local bounds of this sprite.
    ezRectFloat m_localBounds = { 0.0f, 0.0f };

    /// \brief Cutout of the texture. By default, the entire texture is shown.
    ezRectU32 m_cutout = { 0u, 0u };


    ezColor m_color = ezColor::GetWhite();
  };

  /// \brief Updates the internal state of \a sprite.
  KR_ENGINE_API void update(Sprite& sprite);

  /// \brief Initializes a sprite.
  /// \note This is actually just an update call.
  inline ezResult initialize(Sprite& sprite)
  {
    update(sprite);
    return sprite.getNeedsUpdate() ? EZ_FAILURE : EZ_SUCCESS;
  }
}
