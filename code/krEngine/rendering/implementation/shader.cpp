#include <krEngine/rendering/shader.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#include <Foundation/IO/FileSystem/FileReader.h>

static GLuint loadAndCompileShader(GLuint type, const char* filename)
{
  // Read file content
  // =================

  ezFileReader reader;
  if (reader.Open(filename).Failed())
    return 0;

  ezStringBuilder buffer;
  buffer.ReadAll(reader);

  // Create shader and set the source
  // ================================

  const GLchar* source = buffer.GetData();
  GLint sourceLength = buffer.GetCharacterCount();

  auto hShader = glCreateShader(type);
  glCheckLastError();
  glCheck(glShaderSource(hShader, 1, &source, &sourceLength));

  // Compile the shader
  // ==================

  glCheck(glCompileShader(hShader));

  GLint status;
  glCheck(glGetShaderiv(hShader, GL_COMPILE_STATUS, &status));

  if (status == GL_TRUE)
    return hShader;

  EZ_LOG_BLOCK("Shader Compilation Error", filename);

  // Determine size of log
  // =====================

  GLint messageLength = 0;
  glCheck(glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &messageLength));
  buffer.Reserve((ezUInt32)messageLength - 1); // Subtract the null-terminator.

  // Get compilation log
  // ===================

  GLchar* message = const_cast<GLchar*>(buffer.GetData());
  glCheck(glGetShaderInfoLog(hShader, messageLength, &messageLength, message));

  // Pipe the log to the ezLog system
  // ================================

  ezLog::Warning("%s", message);

  // Finally, release the shader again
  // =================================

  glCheck(glDeleteShader(hShader));

  return 0;
}

kr::RefCountedPtr<kr::VertexShader> kr::VertexShader::loadAndCompile(const char* filename)
{
  auto handle = loadAndCompileShader(GL_VERTEX_SHADER, filename);

  if (glIsShader(handle) != GL_TRUE)
  {
    return nullptr;
  }

  auto pVS = EZ_DEFAULT_NEW(VertexShader);
  pVS->m_glHandle = handle;

  return pVS;
}

kr::VertexShader::~VertexShader()
{
  glCheck(glDeleteShader(m_glHandle));
  m_glHandle = 0;
}

kr::RefCountedPtr<kr::FragmentShader> kr::FragmentShader::loadAndCompile(const char* filename)
{
  auto handle = loadAndCompileShader(GL_FRAGMENT_SHADER, filename);

  if (glIsShader(handle) != GL_TRUE)
  {
    return nullptr;
  }
  glCheckLastError();

  auto pFS = EZ_DEFAULT_NEW(FragmentShader);
  pFS->m_glHandle = handle;

  return pFS;
}

kr::FragmentShader::~FragmentShader()
{
  glCheck(glDeleteShader(m_glHandle));
  m_glHandle = 0;
}

kr::RefCountedPtr<kr::ShaderProgram> kr::ShaderProgram::link(VertexShaderPtr pVS,
                                                             FragmentShaderPtr pFS)
{
  if (isNull(pVS))
  {
    EZ_REPORT_FAILURE("Invalid vertex shader pointer.");
    return nullptr;
  }

  if (isNull(pFS))
  {
    EZ_REPORT_FAILURE("Invalid fragment shader pointer.");
    return nullptr;
  }

  auto hProgram = glCreateProgram();

  glCheck(glAttachShader(hProgram, pVS->m_glHandle));
  glCheck(glAttachShader(hProgram, pFS->m_glHandle));

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
    return pProgram;
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

kr::ShaderUniform kr::shaderUniformOf(ShaderProgramPtr pShader, const char* uniformName)
{
  EZ_ASSERT(isValid(pShader), "Invalid shader program.");
  if(isNull(pShader))
  {
    ezLog::Warning("Invalid shader program. Ignoring call.");
    return ShaderUniform();
  }

  auto location = glGetUniformLocation(pShader->m_glHandle, uniformName);
  glCheckLastError();

  if (location == -1)
  {
    ezLog::Warning("Cannot find uniform location for name '%s'.", uniformName);
    return ShaderUniform();
  }

  // Success
  // =======
  ShaderUniform u;
  u.glLocation = location;
  u.pShader = pShader;
  return u;
}

static kr::ShaderProgramPtr g_currentShader;

KR_ENGINE_API ezResult kr::use(ShaderProgramPtr pShader)
{
  EZ_ASSERT(isNull(g_currentShader),
            "Some other shader program is already in use. Is this intentional?");
  if (isNull(pShader))
    return EZ_FAILURE;

  glCheck(glUseProgram(pShader->m_glHandle));
  g_currentShader = pShader;
  return EZ_SUCCESS;
}

void kr::unuse(ShaderProgramPtr pShader)
{
    EZ_ASSERT(g_currentShader == pShader,
              "Calling unuse with a shader that is not in use!");
  if (g_currentShader == pShader)
  {
    glCheck(glUseProgram(0));
    g_currentShader = nullptr;
  }
}
