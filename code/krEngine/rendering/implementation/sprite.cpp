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

static kr::Owned<kr::VertexBuffer> createVertexBuffer(kr::Borrowed<kr::ShaderProgram> pShader)
{
  using namespace kr;

  EZ_LOG_BLOCK("Create VertexBuffer", "Sprite");

  auto pVB = VertexBuffer::create(BufferUsage::StaticDraw,
                                  PrimitiveType::TriangleStrip);

  setupLayout(pVB, pShader, "krSpriteVertex");

  return move(pVB);
}

// static
kr::Owned<kr::ShaderProgram> kr::Sprite::createDefaultShader()
{
  EZ_LOG_BLOCK("Create Shader", "Sprite");

  auto vsName = "<shader>sprite.vs";
  auto fsName = "<shader>sprite.fs";
  auto prg = ShaderProgram::loadAndLink(vsName, fsName);
  if (prg == nullptr)
  {
    EZ_REPORT_FAILURE("Failed to link shaders to program: '%s' and '%s'", vsName, fsName);
  }

  return move(prg);
}

kr::Sprite::Sprite()
{
  m_needUpdate.Add(SpriteComponents::LocalBounds);
  m_needUpdate.Add(SpriteComponents::Cutout);
}

void kr::Sprite::operator=(const Sprite& other)
{
  this->m_needUpdate = other.m_needUpdate;
  ezMemoryUtils::Copy(this->m_vertices, other.m_vertices, 4);
  this->m_pSampler = other.m_pSampler;
  this->m_pTexture = other.m_pTexture;
  this->m_localBounds = other.m_localBounds;
  this->m_cutout = other.m_cutout;
  this->m_color = other.m_color;
  this->m_pShader = other.m_pShader;
  this->m_uTexture = other.m_uTexture;
  this->m_uColor = other.m_uColor;
  this->m_uTransform = other.m_uTransform;
  this->m_uOrigin = other.m_uOrigin;
  this->m_uRotation = other.m_uRotation;
  this->m_uViewMatrix = other.m_uViewMatrix;
  this->m_uProjectionMatrix = other.m_uProjectionMatrix;

  update();
}

kr::Sprite::Sprite(const Sprite& other)
{
  *this = other;
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

ezResult kr::Sprite::initialize(Borrowed<Texture> texture, Borrowed<Sampler> sampler, Borrowed<ShaderProgram> shader)
{
  setTexture(texture);
  setSampler(sampler);
  setShader(shader);
  update();
  bool success = canRender() && !needsUpdate();
  return success ? EZ_FAILURE : EZ_SUCCESS;
}

void kr::Sprite::update()
{
  EZ_LOG_BLOCK("Updating Sprite");

  // Terminate, If There Is No Texture
  // =================================
  if (m_pTexture == nullptr)
  {
    ezLog::Warning("Sprite has no texture yet. Cannot Update.");
    return;
  }

  // Terminate, If There Is No Sampler
  // =================================
  if (m_pSampler == nullptr)
  {
    ezLog::Warning("Sprite has no sampler yet. Cannot Update.");
    return;
  }

  // Terminate, If There Is No Sampler
  // =================================
  if(m_pShader == nullptr)
  {
    ezLog::Warning("Sprite has no shader yet. Cannot Update.");
    return;
  }

  // Create Shader, If Needed
  // ========================
  if(m_needUpdate.IsSet(SpriteComponents::ShaderUniforms))
  {
    m_uOrigin           = shaderUniformOf(m_pShader, "u_origin");
    m_uRotation         = shaderUniformOf(m_pShader, "u_rotation");
    m_uViewMatrix       = shaderUniformOf(m_pShader, "u_view");
    m_uProjectionMatrix = shaderUniformOf(m_pShader, "u_projection");
    m_uColor            = shaderUniformOf(m_pShader, "u_color");
    m_uTexture          = shaderUniformOf(m_pShader, "u_texture");
  }

  // Create VertexBuffer, If Needed
  // ==============================
  if(m_pVertexBuffer == nullptr)
  {
    m_pVertexBuffer = move(createVertexBuffer(m_pShader));
  }

  bool uploadVB = false;

  // Update Cutout
  // =============
  if(m_needUpdate.IsSet(SpriteComponents::Cutout))
  {
    auto& pTexture = m_pTexture;
    auto texWidth = pTexture->getWidth();
    auto texHeight = pTexture->getHeight();

    auto& cutout = m_cutout;
    if (cutout.HasNonZeroArea())
    {
      auto l = float(cutout.x)      / float(texWidth);  // Left.
      auto r = float(cutout.width)  / float(texWidth);  // Right.
      auto t = float(cutout.height) / float(texHeight); // Top.
      auto b = float(cutout.y)      / float(texHeight); // Bottom.

      m_vertices[0].texCoords.Set(l, t);
      m_vertices[1].texCoords.Set(l, b);
      m_vertices[2].texCoords.Set(r, t);
      m_vertices[3].texCoords.Set(r, b);
    }
    else
    {
      cutout.x = 0;
      cutout.y = 0;
      cutout.width = texWidth;
      cutout.height = texHeight;

      m_vertices[0].texCoords.Set(0, 1);
      m_vertices[1].texCoords.Set(0, 0);
      m_vertices[2].texCoords.Set(1, 1);
      m_vertices[3].texCoords.Set(1, 0);
    }

    uploadVB = true;
    m_needUpdate.Remove(SpriteComponents::Cutout);
  }

  // Update Local Bounds
  // ===================
  if(m_needUpdate.IsSet(SpriteComponents::LocalBounds))
  {
    auto& bounds = m_localBounds;

    // If no bounds were set, use the texture dimensions.
    if (!bounds.HasNonZeroArea())
    {
      auto& cutout = m_cutout;
      bounds.width  = float(cutout.width);
      bounds.height = float(cutout.height);
    }

    m_vertices[0].pos.Set(bounds.x,                bounds.y);
    m_vertices[1].pos.Set(bounds.x,                bounds.y + bounds.height);
    m_vertices[2].pos.Set(bounds.x + bounds.width, bounds.y);
    m_vertices[3].pos.Set(bounds.x + bounds.width, bounds.y + bounds.height);

    uploadVB = true;
    m_needUpdate.Remove(SpriteComponents::LocalBounds);
  }

  // Upload VertexBuffer Data If Necessary
  // =====================================
  if (uploadVB)
  {
    uploadData(getVertexBuffer(), getVertices());
  }
}

bool kr::Sprite::canRender()
{
  return getTexture() != nullptr
      && getSampler() != nullptr
      && getShader()  != nullptr;
}
