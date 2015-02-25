#pragma once
#include <krEngine/referenceCounting.h>
#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/vertexBuffer.h>
#include <krEngine/rendering/shader.h>

namespace kr
{
  class KR_ENGINE_API Sprite
  {
  private: // *** Data
    /// \brief \c true if the state was altered without updating.
    bool m_needsUpdate = false;

    ShaderUniform m_uTexture;
    ShaderUniform m_uColor;

    RefCountedPtr<ShaderProgram> m_pShader;

    RefCountedPtr<VertexBuffer> m_pVertexBuffer;

    /// \brief Handle to the texture used by this sprite.
    RefCountedPtr<Texture> m_pTexture;

    /// \brief The bounds (or "view") of this sprite into the texture.
    ///
    /// By default, the entire texture is used.
    ezRectU32 m_bounds = { 0u, 0u };

    ezColor m_color = ezColor::GetWhite();

  public: // *** Accessors/Mutators
    void setColor(ezColor c) { m_color = move(c); }
    ezColor getColor() const { return m_color; }

    /// \brief Set the current texture.
    void setTexture(RefCountedPtr<Texture> pTex)
    {
      m_needsUpdate = true;
      m_pTexture = pTex;
    }

    /// \brief Gets the handle to the current texture.
    RefCountedPtr<Texture> getTexture() const
    {
      return m_pTexture;
    }

    /// \brief Set the bounds (or "view") of this sprite into the texture.
    void setBounds(ezRectU32 newBounds)
    {
      m_needsUpdate = true;
      m_bounds = move(newBounds);
    }

    /// \brief Get the bounds of this sprite.
    ezRectU32 getBounds() const
    {
      return m_bounds;
    }

  public: // *** Friends
    friend KR_ENGINE_API void update(Sprite& sprite);
    friend KR_ENGINE_API void draw(Sprite& sprite);
  };

  /// \brief Updates the internal state of \a sprite.
  KR_ENGINE_API void update(Sprite& sprite);

  /// \brief Draws the given \a sprite.
  KR_ENGINE_API void draw(Sprite& sprite);
}
