#include <krEngine/pch.h>
#include <krEngine/pch.h>
#include <krEngine/rendering/sprite.h>
#include <krEngine/rendering/shader.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

EZ_BEGIN_STATIC_REFLECTED_TYPE(krSpriteVertex, ezNoBase, 1, ezRTTINoAllocator);
  EZ_BEGIN_PROPERTIES
    // We use the names used in the shader here.
    EZ_MEMBER_PROPERTY("vs_position",  pos),
    EZ_MEMBER_PROPERTY("vs_texCoords", texCoords)
  EZ_END_PROPERTIES
EZ_END_STATIC_REFLECTED_TYPE();

static kr::VertexBufferPtr createVertexBuffer(kr::ShaderProgramPtr pShader)
{
  using namespace kr;

  EZ_LOG_BLOCK("Create VertexBuffer", "Sprite");

  auto pVB = VertexBuffer::create(BufferUsage::StaticDraw,
                                  PrimitiveType::TriangleStrip);

  setupLayout(pVB, pShader, "krSpriteVertex");

  return pVB;
}

static kr::ShaderProgramPtr createSpriteShader()
{
  using namespace kr;

  EZ_LOG_BLOCK("Create Shader", "Sprite");

  auto vsName = "<shader>sprite.vs";
  auto fsName = "<shader>sprite.fs";
  auto prg = ShaderProgram::loadAndLink(vsName, fsName);
  if (isNull(prg))
  {
    EZ_REPORT_FAILURE("Failed to link shaders to program: '%s' and '%s'", vsName, fsName);
    return nullptr;
  }

  return prg;
}

kr::Sprite::Sprite()
{
  m_needUpdate.Add(SpriteComponents::LocalBounds);
  m_needUpdate.Add(SpriteComponents::Cutout);
}

void kr::Sprite::setLocalBounds(ezRectFloat newLocalBounds)
{
  m_localBounds = move(newLocalBounds);
  m_needUpdate.Add(SpriteComponents::LocalBounds);
}

void kr::Sprite::setCutout(ezRectU32 newCutout)
{
  m_cutout = move(newCutout);
  m_needUpdate.Add(SpriteComponents::Cutout);
}

void kr::update(Sprite& sprite)
{
  EZ_LOG_BLOCK("Updating Sprite");

  // Create Shader, If Needed
  // ========================
  if (isNull(sprite.m_pShader))
  {
    sprite.m_pShader = createSpriteShader();
    sprite.m_uOrigin           = shaderUniformOf(sprite.m_pShader, "u_origin");
    sprite.m_uRotation         = shaderUniformOf(sprite.m_pShader, "u_rotation");
    sprite.m_uViewMatrix       = shaderUniformOf(sprite.m_pShader, "u_view");
    sprite.m_uProjectionMatrix = shaderUniformOf(sprite.m_pShader, "u_projection");
    sprite.m_uColor            = shaderUniformOf(sprite.m_pShader, "u_color");
    sprite.m_uTexture          = shaderUniformOf(sprite.m_pShader, "u_texture");
  }

  // Create VertexBuffer, If Needed
  // ==============================
  if (isNull(sprite.m_pVertexBuffer))
  {
    sprite.m_pVertexBuffer = createVertexBuffer(sprite.m_pShader);
  }

  // Terminate, If There Is No Texture
  // =================================
  if (isNull(sprite.m_pTexture))
  {
    ezLog::Warning("Sprite has no texture yet. Ignoring.");
    return;
  }

  // Create Sampler, If Needed
  // =========================
  if (isNull(sprite.m_pSampler))
  {
    sprite.m_pSampler = Sampler::create();
    ezLog::Debug("Using default texture sampler for sprite.");
  }

  bool uploadVB = false;

  // Update Cutout
  // =============
  if(sprite.m_needUpdate.IsSet(SpriteComponents::Cutout))
  {
    auto& pTexture = sprite.m_pTexture;
    auto texWidth = pTexture->getWidth();
    auto texHeight = pTexture->getHeight();

    auto& cutout = sprite.m_cutout;
    if (cutout.HasNonZeroArea())
    {
      auto l = float(cutout.x)      / float(texWidth);  // Left.
      auto r = float(cutout.width)  / float(texWidth);  // Right.
      auto t = float(cutout.height) / float(texHeight); // Top.
      auto b = float(cutout.y)      / float(texHeight); // Bottom.

      sprite.m_vertices[0].texCoords.Set(l, t);
      sprite.m_vertices[1].texCoords.Set(l, b);
      sprite.m_vertices[2].texCoords.Set(r, t);
      sprite.m_vertices[3].texCoords.Set(r, b);
    }
    else
    {
      cutout.x = 0;
      cutout.y = 0;
      cutout.width = texWidth;
      cutout.height = texHeight;

      sprite.m_vertices[0].texCoords.Set(0, 1);
      sprite.m_vertices[1].texCoords.Set(0, 0);
      sprite.m_vertices[2].texCoords.Set(1, 1);
      sprite.m_vertices[3].texCoords.Set(1, 0);
    }

    uploadVB = true;
    sprite.m_needUpdate.Remove(SpriteComponents::Cutout);
  }

  // Update Local Bounds
  // ===================
  if(sprite.m_needUpdate.IsSet(SpriteComponents::LocalBounds))
  {
    auto& bounds = sprite.m_localBounds;

    // If no bounds were set, use the texture as bounds.
    if (!bounds.HasNonZeroArea())
    {
      auto& cutout = sprite.m_cutout;
      bounds.x      = float(cutout.x);
      bounds.y      = float(cutout.x);
      bounds.width  = float(cutout.width);
      bounds.height = float(cutout.height);
    }

    sprite.m_vertices[0].pos.Set(bounds.x,                bounds.y);
    sprite.m_vertices[1].pos.Set(bounds.x,                bounds.y + bounds.height);
    sprite.m_vertices[2].pos.Set(bounds.x + bounds.width, bounds.y);
    sprite.m_vertices[3].pos.Set(bounds.x + bounds.width, bounds.y + bounds.height);

    uploadVB = true;
    sprite.m_needUpdate.Remove(SpriteComponents::LocalBounds);
  }

  // Upload VertexBuffer Data If Necessary
  // =====================================
  if (uploadVB)
  {
    uploadData(sprite.getVertexBuffer(), sprite.getVertices());
  }
}
