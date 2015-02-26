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

  vertices[0].pos.Set(-0.5f, -0.5f); // Lower Left
  vertices[1].pos.Set(-0.5f,  0.5f); // Upper Left
  vertices[2].pos.Set( 0.5f, -0.5f); // Lower Right
  vertices[3].pos.Set( 0.5f,  0.5f); // Upper Right

  vertices[0].texCoords.Set(0.0f, 1.0f); // Upper Left
  vertices[1].texCoords.Set(0.0f, 0.0f); // Lower Left
  vertices[2].texCoords.Set(1.0f, 1.0f); // Upper Right
  vertices[3].texCoords.Set(1.0f, 0.0f); // Lower Right

  uploadData(pVB, ezMakeArrayPtr(vertices));

  return pVB;
}

static kr::ShaderProgramPtr createSpriteShader()
{
  using namespace kr;

  EZ_LOG_BLOCK("Create Shader", "Sprite");

  auto vsName = "<shader>sprite.vs";
  auto vs = VertexShader::loadAndCompile(vsName);
  if (isNull(vs))
  {
    EZ_REPORT_FAILURE("Failed to load vertex shader '%s'.", vsName);
    return nullptr;
  }

  auto fsName = "<shader>sprite.fs";
  auto fs = FragmentShader::loadAndCompile(fsName);
  if (isNull(fs))
  {
    EZ_REPORT_FAILURE("Failed to load fragment shader '%s'.", fsName);
    return nullptr;
  }

  auto prg = ShaderProgram::link(vs, fs);
  if (isNull(prg))
  {
    EZ_REPORT_FAILURE("Failed to link shaders to program: '%s' and '%s'", vsName, fsName);
    return nullptr;
  }

  return prg;
}

void kr::update(Sprite& sprite)
{
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

void kr::draw(Sprite& sprite)
{
  // If the sprite needs an update, try doing that.
  if (sprite.m_needsUpdate)
    update(sprite);

  // If it still needs an update, something went wrong there.
  if (sprite.m_needsUpdate)
    return;

  // If there is no shader, we cannot draw.
  if (isNull(sprite.m_pShader))
    return;

  // Set Active Shader
  // =================
  use(sprite.m_pShader);
  KR_ON_SCOPE_EXIT{ unuse(sprite.m_pShader); };

  // Set Active Vertex Buffer
  // ========================
  use(sprite.m_pVertexBuffer, sprite.m_pShader);
  KR_ON_SCOPE_EXIT{ unuse(sprite.m_pVertexBuffer, sprite.m_pShader); };

  // Update Uniforms
  // ===============
  uploadData(sprite.m_uColor, sprite.m_color);
  uploadData(sprite.m_uTexture, TextureSlot(0));
}
