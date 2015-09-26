#include <krEngine/pch.h>
#include <krEngine/rendering/shader.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#include <Foundation/IO/FileSystem/FileReader.h>

using ShaderBindings = ezHybridArray<kr::Borrowed<const kr::ShaderProgram>, 8>;

static ShaderBindings* g_pShaderBindings;
static bool g_initialized = false;

EZ_BEGIN_SUBSYSTEM_DECLARATION(krEngine, Shaders)
  BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORE_STARTUP
  {
    g_pShaderBindings = new (m_mem_shaderBindings) ShaderBindings();

    g_initialized = true;
  }

  ON_CORE_SHUTDOWN
  {
    if (g_pShaderBindings->GetCount() > 0)
    {
      ezLog::Error("There are still %u active shader program bindings!",
                    g_pShaderBindings->GetCount());
    }

    g_pShaderBindings->~ShaderBindings();
    g_pShaderBindings = nullptr;

    g_initialized = false;
  }

private:
  ezUByte m_mem_shaderBindings[sizeof(ShaderBindings)];
EZ_END_SUBSYSTEM_DECLARATION

static void logCompileStatus(GLuint hShader)
{
  ezHybridArray<GLchar, 512> log;

  // Determine size of log
  // =====================

  GLint messageLength = 0;
  glCheck(glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &messageLength));
  log.SetCountUninitialized(messageLength);

  // Get compilation log
  // ===================

  glCheck(glGetShaderInfoLog(hShader, messageLength, &messageLength, log.GetData()));

  // Pipe the message through to the ezLog system
  // ============================================

  ezLog::Warning("%s", log.GetData());
}

static GLuint loadAndCompileShader(GLuint type, ezStringView fileName)
{
  ezStringBuilder sbFileName(fileName);

  EZ_LOG_BLOCK("Loading and Compiling Shader From File", sbFileName);

  // Read file content
  // =================

  ezStringBuilder code;
  {
    ezFileReader reader;
    if(reader.Open(sbFileName).Failed())
      return 0;

    code.ReadAll(reader);
  }

  // Create shader and set the source
  // ================================

  const GLchar* source = code.GetData();
  GLint sourceLength = code.GetCharacterCount();

  auto hShader = glCreateShader(type);
  glCheckLastError();
  glCheck(glShaderSource(hShader, 1, &source, &sourceLength));

  // Compile the shader
  // ==================

  glCheck(glCompileShader(hShader));

  // Get the compilation status.
  GLint status;
  glCheck(glGetShaderiv(hShader, GL_COMPILE_STATUS, &status));

  // If the compilation was a success, return the handle.
  if (status == GL_TRUE)
    return hShader;

  // Shader Compilation Was Not Successful
  // =====================================

  // Log the status message.
  logCompileStatus(hShader);

  // Release the shader.
  glCheck(glDeleteShader(hShader));

  return 0;
}

kr::Owned<kr::VertexShader> kr::VertexShader::loadAndCompile(ezStringView fileName)
{
  auto handle = loadAndCompileShader(GL_VERTEX_SHADER, fileName);

  if (glIsShader(handle) != GL_TRUE)
  {
    return nullptr;
  }

  auto vs = EZ_DEFAULT_NEW(VertexShader);
  vs->m_glHandle = handle;
  vs->m_resourceId = fileName;

  return own(vs, [](VertexShader* vs) { EZ_DEFAULT_DELETE(vs); });
}

kr::VertexShader::~VertexShader()
{
  glCheck(glDeleteShader(m_glHandle));
  m_glHandle = 0;
}

kr::Owned<kr::FragmentShader> kr::FragmentShader::loadAndCompile(ezStringView fileName)
{
  auto handle = loadAndCompileShader(GL_FRAGMENT_SHADER, fileName);

  if (glIsShader(handle) != GL_TRUE)
  {
    return nullptr;
  }
  glCheckLastError();

  auto fs = EZ_DEFAULT_NEW(FragmentShader);
  fs->m_glHandle = handle;
  fs->m_resourceId = fileName;

  return own(fs, [](FragmentShader* fs) { EZ_DEFAULT_DELETE(fs); });
}

kr::FragmentShader::~FragmentShader()
{
  glCheck(glDeleteShader(m_glHandle));
  m_glHandle = 0;
}

kr::Owned<kr::ShaderProgram> kr::ShaderProgram::link(Borrowed<VertexShader> vs,
                                                     Borrowed<FragmentShader> fs)
{
  if (vs == nullptr)
  {
    EZ_REPORT_FAILURE("Invalid vertex shader pointer.");
    return nullptr;
  }

  if (fs == nullptr)
  {
    EZ_REPORT_FAILURE("Invalid fragment shader pointer.");
    return nullptr;
  }

  auto hProgram = glCreateProgram();

  glCheck(glAttachShader(hProgram, vs->m_glHandle));
  glCheck(glAttachShader(hProgram, fs->m_glHandle));

  // Link the program
  // ================

  glCheck(glLinkProgram(hProgram));

  // Get the link status
  // ===================

  GLint status;
  glCheck(glGetProgramiv(hProgram, GL_LINK_STATUS, &status));
  // If linking succeeded, return success.
  if (status == GL_TRUE)
  {
    auto pProgram = EZ_DEFAULT_NEW(ShaderProgram);
    pProgram->m_glHandle = hProgram;
    return own(pProgram, [](ShaderProgram* p){ EZ_DEFAULT_DELETE(p); });
  }

  // Get log message length
  // ======================

  GLint messageLength = 0;
  glCheck(glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &messageLength));

  // Get log message
  // ===============

  ezStringBuilder buffer;
  buffer.Reserve((ezUInt32)messageLength - 1);

  auto message = const_cast<GLchar*>(buffer.GetData());
  glCheck(glGetProgramInfoLog(hProgram, messageLength, &messageLength, message));

  // Forward log message to ezLog
  // ============================

  ezLog::Warning("%s", message);

  // Release the allocated object
  // ============================

  glCheck(glDeleteProgram(hProgram));

  return nullptr;
}

kr::ShaderProgram::~ShaderProgram()
{
  glCheck(glDeleteProgram(m_glHandle));
  m_glHandle = 0;
}

kr::ShaderUniform kr::shaderUniformOf(Borrowed<ShaderProgram> pShader, ezStringView uniformName)
{
  EZ_ASSERT_DEV(pShader != nullptr, "Invalid shader program.");
  if(pShader == nullptr)
  {
    ezLog::Warning("Invalid shader program. Ignoring call.");
    return ShaderUniform();
  }

  ezStringBuilder sbUniformName(uniformName);
  auto location = glGetUniformLocation(pShader->m_glHandle, sbUniformName);
  glCheckLastError();

  if (location == -1)
  {
    ezLog::Warning("Cannot find uniform location for name '%s'.", sbUniformName);
    return ShaderUniform();
  }

  // Success
  // =======
  ShaderUniform u;
  u.glLocation = location;
  u.pShader = pShader;
  return u;
}

ezResult kr::bind(Borrowed<const ShaderProgram> pShader)
{
  if (pShader == nullptr)
  {
    ezLog::Warning("Cannot bind nullptr as shader program. Ignoring.");
    return EZ_FAILURE;
  }

  auto handle = pShader->getGlHandle();

  // Set the active shader program.
  glCheck(glUseProgram(handle));

  // Save the shader program.
  g_pShaderBindings->ExpandAndGetRef() = move(pShader);

  return EZ_SUCCESS;
}

ezResult kr::restoreLastShaderProgram()
{
  if (g_pShaderBindings->IsEmpty())
  {
    ezLog::Warning("No last texture 2D to restore!");
    return EZ_FAILURE;
  }

  // Drop the current binding.
  g_pShaderBindings->PopBack();

  if(g_pShaderBindings->IsEmpty())
  {
    glCheck(glUseProgram(0));
    return EZ_SUCCESS;
  }

  // Get the handle of the current binding.
  auto handle = g_pShaderBindings->PeekBack()->getGlHandle();

  // And actually bind it again.
  glCheck(glUseProgram(handle));

  return EZ_SUCCESS;
}

static bool checkUniformUploadPreconditions(const kr::ShaderUniform& uniform)
{
  if (uniform.pShader == nullptr)
  {
    ezLog::Warning("Invalid shader object.");
    return false;
  }
  if (uniform.glLocation == -1)
  {
    ezLog::Warning("Invalid uniform location.");
    return false;
  }

  return true;
}

#define PRECONDITIONS_FOR_UPLOAD(uniform, type)  \
  EZ_LOG_BLOCK("Uploading Uniform Value", type); \
  if(!checkUniformUploadPreconditions(uniform)) return EZ_FAILURE;


ezResult kr::uploadData(const ShaderUniform& uniform,
                        ezColor value)
{
  PRECONDITIONS_FOR_UPLOAD(uniform, "Color");

  glCheck(glProgramUniform4fv(uniform.pShader->getGlHandle(),
                              uniform.glLocation,
                              1, value.GetData()));

  return EZ_SUCCESS;
}

ezResult kr::uploadData(const ShaderUniform& uniform,
                        TextureSlot slot)
{
  PRECONDITIONS_FOR_UPLOAD(uniform, "Texture");

  glCheck(glProgramUniform1i(uniform.pShader->getGlHandle(),
                             uniform.glLocation,
                             slot.value));

  return EZ_SUCCESS;
}

ezResult kr::uploadData(const ShaderUniform& uniform, const ezMat4& matrix)
{
  PRECONDITIONS_FOR_UPLOAD(uniform, "Matrix4x4");

  glCheck(glProgramUniformMatrix4fv(uniform.pShader->getGlHandle(), // Shader program handle.
                                    uniform.glLocation,             // Uniform location.
                                    1,                              // Number of matrices.
                                    GL_FALSE,                       // Transpose?
                                    matrix.m_fElementsCM));         // Matrix data.

  return EZ_SUCCESS;
}

ezResult kr::uploadData(const ShaderUniform& uniform, const ezVec2& vec)
{
  PRECONDITIONS_FOR_UPLOAD(uniform, "Vec2");

  glCheck(glProgramUniform2fv(uniform.pShader->getGlHandle(), // Shader program handle.
                              uniform.glLocation,             // Uniform location.
                              1,                              // Number of vectors.
                              vec.GetData()));                // Vector data.

  return EZ_SUCCESS;
}

ezResult kr::uploadData(const ShaderUniform& uniform, const ezAngle& angle)
{
  PRECONDITIONS_FOR_UPLOAD(uniform, "Angle");

  glCheck(glProgramUniform1f(uniform.pShader->getGlHandle(), // Shader program handle.
                             uniform.glLocation,             // Uniform location.
                             angle.GetRadian()));            // Matrix data.

  return EZ_SUCCESS;
}

#undef PRECONDITIONS_FOR_UPLOAD
