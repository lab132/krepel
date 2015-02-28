#include <krEngine/rendering/implementation/extractionData.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

void kr::draw(SpriteData& sprite,
              const ezMat4& viewMatrix,
              const ezMat4& projectionMatrix)
{
  EZ_LOG_BLOCK("Drawing Sprite");

  // If there is no shader, we cannot draw.
  if (isNull(sprite.pShader))
  {
    ezLog::Warning("No shader to draw with.");
    return;
  }

  TextureSlot textureSlot(0);

  // Set Active Shader
  // =================
  KR_RAII_BIND_SHADER(sprite.pShader);
  KR_RAII_BIND_VERTEX_BUFFER(sprite.pVertexBuffer, sprite.pShader);
  KR_RAII_BIND_SAMPLER(sprite.pSampler, textureSlot);
  KR_RAII_BIND_TEXTURE_2D(sprite.pTexture, textureSlot);

  // Update Uniforms
  // ===============
  uploadData(sprite.uColor, sprite.color);
  uploadData(sprite.uTexture, textureSlot);
  uploadData(sprite.uViewMatrix, viewMatrix);
  uploadData(sprite.uProjectionMatrix, projectionMatrix);

  glCheck(glDrawArrays((GLenum)sprite.pVertexBuffer->getPrimitive(), 0, 4));
}
