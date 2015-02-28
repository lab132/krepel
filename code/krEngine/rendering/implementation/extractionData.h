#pragma once
#include <krEngine/transform2D.h>
#include <krEngine/rendering/vertexBuffer.h>
#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/sprite.h>

namespace kr
{
  enum class ExtractionDataType
  {
    Invalid,
    Sprite,
  };

  struct ExtractionData
  {
    ExtractionDataType type;
    size_t byteCount;
  };

  //////////////////////////////////////////////////////////////////////////

  struct SpriteData : public ExtractionData
  {
    /// \note This is required.
    enum { Type = ExtractionDataType::Sprite };

    ShaderProgramPtr pShader;
    VertexBufferPtr pVertexBuffer;
    TexturePtr pTexture;
    SamplerPtr pSampler;
    ShaderUniform uTexture;
    ShaderUniform uColor;
    ShaderUniform uViewMatrix;
    ShaderUniform uProjectionMatrix;

    Transform2D transform;
    ezColor color;
  };

  void draw(SpriteData& sprite,
            const ezMat4& viewMatrix,
            const ezMat4& projectionMatrix);
}
