#include <krEngine/rendering/vertexBuffer.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#include <Foundation/Reflection/Reflection.h>

/// \note Not all types are supported.
static void getSizeInfo(ezVariant::Type::Enum t, GLenum& type, GLint& numComponents)
{
  numComponents = 1;
  switch(t)
  {
  case ezVariant::Type::Int8:
    type = GL_BYTE;
    break;
  case ezVariant::Type::UInt8:
    type = GL_UNSIGNED_BYTE;
    break;
  case ezVariant::Type::Int16:
    type = GL_SHORT;
    break;
  case ezVariant::Type::UInt16:
    type = GL_UNSIGNED_SHORT;
    break;
  case ezVariant::Type::Int32:
    type = GL_INT;
    break;
  case ezVariant::Type::UInt32:
    type = GL_UNSIGNED_INT;
    break;
  case ezVariant::Type::Float:
    type = GL_FLOAT;
    break;
  case ezVariant::Type::Double:
    type = GL_DOUBLE;
    break;
  case ezVariant::Type::Vector2:
    numComponents = 2;
    type = GL_FLOAT;
    break;
  case ezVariant::Type::Vector3:
    numComponents = 3;
    type = GL_FLOAT;
    break;
  case ezVariant::Type::Vector4:
  case ezVariant::Type::Color:
  case ezVariant::Type::Quaternion:
    numComponents = 4;
    type = GL_FLOAT;
    break;
  case ezVariant::Type::Matrix3:
    numComponents = 3 * 3;
    type = GL_FLOAT;
    break;
  case ezVariant::Type::Matrix4:
    numComponents = 4 * 4;
    type = GL_FLOAT;
    break;
  default:
    EZ_REPORT_FAILURE("Unsupported input type.");
    type = GL_FALSE;
    numComponents = -1;
    break;
  }
}

kr::RefCountedPtr<kr::VertexBuffer> kr::VertexBuffer::create(BufferUsage usage,
                                                             PrimitiveType primitive)
{
  GLuint handle;
  glGenBuffers(1, &handle);

  if (handle == 0)
  {
    // Failure
    // =======

    ezLog::Warning("Failed to create OpenGL vertex buffer. "
                   "Did you forget to create a rendering context?");
    return nullptr;
  }

  // Success
  // =======

  auto pVertexBuffer = EZ_DEFAULT_NEW(VertexBuffer);
  pVertexBuffer->m_glHandle = handle;
  pVertexBuffer->m_usage = usage;
  pVertexBuffer->m_primitive = primitive;
  return pVertexBuffer;
}

kr::VertexBuffer::~VertexBuffer()
{
  glCheck(glDeleteBuffers(1, &m_glHandle));
  m_glHandle = 0;
}

ezResult kr::setupLayout(VertexBufferPtr pVertBuffer,
                         ShaderProgramPtr pProgram,
                         const char* layoutTypeName)
{
  EZ_LOG_BLOCK("Setup Vertex Buffer Layout");

  if (isNull(pVertBuffer))
  {
    ezLog::Warning("Vertex buffer is nullptr. Aborting.");
    return EZ_FAILURE;
  }

  if (isNull(pProgram))
  {
    ezLog::Warning("Shader program is nullptr. Aborting.");
    return EZ_FAILURE;
  }

  // Use reflection to find the actual type data.
  auto pLayout = ezRTTI::FindTypeByName(layoutTypeName);

  if (pLayout == nullptr)
  {
    ezLog::Warning(
        "Given layout type name is not the name of a reflected type (ezRTTI)",
        layoutTypeName);
    return EZ_FAILURE;
  }

  // Vertex Array Object (VAO)
  // =========================
  GLuint vao = 0;

  // Check if we already
  for (ezUInt32 i = 0; i < pVertBuffer->m_Vaos.GetCount(); ++i)
  {
    auto& pair = pVertBuffer->m_Vaos[i];
    if (pair.pProgram == pProgram)
    {
      ezLog::Debug("Overwriting existing vertex buffer layout binding.");
      vao = pair.hVao;
      break;
    }
  }

  // If we have an existing vertex array object.
  if (vao == 0)
  {
    // Create a new one.
    glCheck(glGenVertexArrays(1, &vao));

    // Remember it.
    auto& entry = pVertBuffer->m_Vaos.ExpandAndGetRef();
    entry.hVao = vao;
    entry.pProgram = pProgram;
  }

  glCheck(glBindVertexArray(vao));

  // Bind Vertex Buffer Object
  // =========================
  auto hVbo = pVertBuffer->m_glHandle;
  auto vboTarget = static_cast<GLenum>(pVertBuffer->getTarget());

  glCheck(glBindBuffer(vboTarget, hVbo));

  // Bind Vertex Attributes
  // ======================
  // The size of the layout type.
  auto stride = pLayout->GetTypeSize();

  // The properties of the layout type.
  auto& props = pLayout->GetProperties();

  auto result = EZ_SUCCESS;
  size_t offset = 0;
  auto hProgram = pProgram->m_glHandle;

  for (ezUInt32 i = 0; i < props.GetCount(); ++i)
  {
    auto pProp = props[i];

    // We only need the members, such as ezColor::r.
    if (pProp->GetCategory() != ezAbstractProperty::Member)
      continue;

    // The name of the attribute in the layout struct.
    auto attributeName = static_cast<ezAbstractMemberProperty*>(pProp)->GetPropertyName();

    // The reflectable member attribute.
    auto pAttribute = static_cast<ezAbstractMemberProperty*>(pProp)->GetPropertyType();

    // Size of the attribute (in bytes).
    auto attributeSize = pAttribute->GetTypeSize();

    // The variant type of the attribute.
    // used to determine the corresponding GL type and number of components.
    auto attributeType = pAttribute->GetVariantType();

    // The reference to the attribute within the program.
    auto location = glGetAttribLocation(hProgram, attributeName);
    glCheckLastError();

    if (location != -1)
    {
      // Set Vertex Attributes for the Found Location
      // ============================================

      GLenum type;
      GLint numComponents;
      getSizeInfo(attributeType, type, numComponents);

      glCheck(glEnableVertexAttribArray(location));
      glCheck(glVertexAttribPointer(location,
                                    numComponents,
                                    type,
                                    GL_FALSE,
                                    stride,
                                    (void*)offset));
    }
    else
    {
      ezLog::Warning("Attribute location of '%s' not found.", attributeName);
      result = EZ_FAILURE;
    }

    offset += attributeSize;
  }

  /// \todo Do the following two calls using some kind of ON_SCOPE_EXIT mechanism.

  glCheck(glBindVertexArray(0));
  glCheck(glBindBuffer(vboTarget, 0));

  return result;
}

ezResult kr::uploadData(VertexBufferPtr pVertBuffer,
                        ezUInt32 byteCount,
                        const void* bytes,
                        ezUInt32 offet)
{
  EZ_LOG_BLOCK("Upload Vertex Buffer Data");

  if (isNull(pVertBuffer))
  {
    ezLog::Warning("Invalid vertex buffer object.");
    return EZ_FAILURE;
  }

  auto handle = pVertBuffer->m_glHandle;
  auto target = static_cast<GLenum>(pVertBuffer->getTarget());
  auto usage = static_cast<GLenum>(pVertBuffer->getUsage());

  EZ_ASSERT(glIsBuffer(handle) == GL_TRUE, "Invalid vertex buffer");

  glCheck(glBindBuffer(target, handle));
  // TODO Could optimize here by using glBufferSubData instead.
  glCheck(glBufferData(target, (GLsizeiptr)byteCount, bytes, usage));
  glCheck(glBindBuffer(target, 0));

  return EZ_SUCCESS;
}

ezResult kr::use(VertexBufferPtr pVertBuffer, ShaderProgramPtr pProgram)
{
  if (isNull(pVertBuffer))
  {
    ezLog::Warning("Invalid vertex buffer.");
    return EZ_FAILURE;
  }

  if (isNull(pProgram))
  {
    ezLog::Warning("Invalid shader program.");
    return EZ_FAILURE;
  }

  for (auto& pair : pVertBuffer->m_Vaos)
  {
    if (pair.pProgram == pProgram)
    {
      glCheck(glBindVertexArray(pair.hVao));
    return EZ_SUCCESS;
    }
  }

  ezLog::Warning("No vertex array object found "
                 "in given vertex buffer "
                 "for given program.");

  glCheck(glBindVertexArray(0));
  return EZ_FAILURE;
}

void kr::unuse(VertexBufferPtr pVertBuffer, ShaderProgramPtr pProgram)
{
  EZ_IGNORE_UNUSED(pVertBuffer);
  EZ_IGNORE_UNUSED(pProgram);

  glCheck(glBindVertexArray(0));
}
