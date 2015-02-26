#include <krEngine/rendering/sprite.h>
#include <krEngine/rendering/shader.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#include <Foundation/Reflection/Reflection.h>

namespace
{
  struct SpriteVertex
  {
    // *** Per-Vertex Data
    ezVec2 pos = ezVec2::ZeroVector();
    ezVec2 texCoords = ezVec2::ZeroVector();
  };
}

EZ_DECLARE_REFLECTABLE_TYPE(EZ_NO_LINKAGE, SpriteVertex);

EZ_BEGIN_STATIC_REFLECTED_TYPE(SpriteVertex, ezNoBase, 1, ezRTTINoAllocator);
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

  setupLayout(pVB, pShader, "SpriteVertex");

  SpriteVertex vertices[4];

  /// \todo Make use of bounds here.

  const auto tw = 512;
  const auto th = 512;

  const auto x = 32;
  const auto y = 32;
  const auto w = x + 256;
  const auto h = y + 256;

  auto left   = float(x) / float(tw);
  auto right  = float(w) / float(tw);
  auto top    = float(h) / float(th);
  auto bottom = float(y) / float(th);

  vertices[0].pos.Set(left  * 2 - 1,bottom * 2 - 1); // Lower Left
  vertices[1].pos.Set(left  * 2 - 1,top    * 2 - 1); // Upper Left
  vertices[2].pos.Set(right * 2 - 1,bottom * 2 - 1); // Lower Right
  vertices[3].pos.Set(right * 2 - 1,top    * 2 - 1); // Upper Right

  vertices[0].texCoords.Set(left,  top);
  vertices[1].texCoords.Set(left,  bottom);
  vertices[2].texCoords.Set(right, top);
  vertices[3].texCoords.Set(right, bottom);

  uploadData(pVB, ezMakeArrayPtr(vertices));

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

void kr::update(Sprite& sprite)
{
  EZ_LOG_BLOCK("Updating Sprite");

  if (isNull(sprite.m_pShader))
  {
    sprite.m_pShader = createSpriteShader();
    sprite.m_uColor = shaderUniformOf(sprite.m_pShader, "u_color");
    sprite.m_uTexture = shaderUniformOf(sprite.m_pShader, "u_texture");
  }

  if (isNull(sprite.m_pVertexBuffer))
  {
    sprite.m_pVertexBuffer = createVertexBuffer(sprite.m_pShader);
  }

  if (isNull(sprite.m_pTexture))
  {
    ezLog::Warning("Sprite has no texture yet. Ignoring.");
    return;
  }

  if (isNull(sprite.m_pSampler))
  {
    sprite.m_pSampler = Sampler::create();
    ezLog::Debug("Using default texture sampler for sprite.");
  }

  // Update Bounds
  // =============

  auto& pTexture = sprite.m_pTexture;
  auto& bounds = sprite.m_bounds;
  if (bounds.HasNonZeroArea())
  {
    bounds.x = 0;
    bounds.y = 0;
    bounds.width = pTexture->getWidth();
    bounds.height = pTexture->getHeight();
  }

  // Finish Updating
  // ===============
  sprite.m_needsUpdate = false;
}
