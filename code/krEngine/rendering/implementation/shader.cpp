#include <krEngine/rendering/shader.h>

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
  glShaderSource(hShader, 1, &source, &sourceLength);

  // Compile the shader
  // ==================

  glCompileShader(hShader);

  GLint status;
  glGetShaderiv(hShader, GL_COMPILE_STATUS, &status);

  if (status == GL_TRUE)
    return hShader;

  EZ_LOG_BLOCK("Shader Compilation Error", filename);

  // Determine size of log
  // =====================

  GLint messageLength = 0;
  glGetShaderiv(hShader, GL_INFO_LOG_LENGTH, &messageLength);
  buffer.Reserve((ezUInt32)messageLength - 1); // Subtract the null-terminator.

  // Get compilation log
  // ===================

  GLchar* message = const_cast<GLchar*>(buffer.GetData());
  glGetShaderInfoLog(hShader, messageLength, &messageLength, message);

  // Pipe the log to the ezLog system
  // ================================

  ezLog::Warning("%s", message);

  // Finally, release the shader again
  // =================================

  glDeleteShader(hShader);

  return 0;
}

kr::RefCountedPtr<kr::VertexShader> kr::VertexShader::loadAndCompile(const char* filename)
{
  auto handle = loadAndCompileShader(GL_VERTEX_SHADER, filename);

  if (glIsShader(handle) != GL_TRUE)
    return nullptr;

  auto pVS = EZ_DEFAULT_NEW(VertexShader);
  pVS->m_glHandle = handle;

  return pVS;
}

kr::VertexShader::~VertexShader()
{
  glDeleteShader(m_glHandle);
  m_glHandle = 0;
}

kr::RefCountedPtr<kr::FragmentShader> kr::FragmentShader::loadAndCompile(const char* filename)
{
  auto handle = loadAndCompileShader(GL_FRAGMENT_SHADER, filename);

  if (glIsShader(handle) != GL_TRUE)
    return nullptr;

  auto pFS = EZ_DEFAULT_NEW(FragmentShader);
  pFS->m_glHandle = handle;

  return pFS;
}

kr::FragmentShader::~FragmentShader()
{
  glDeleteShader(m_glHandle);
  m_glHandle = 0;
}

kr::RefCountedPtr<kr::ShaderProgram>
kr::ShaderProgram::link(RefCountedPtr<VertexShader> pVS,
                        RefCountedPtr<FragmentShader> pFS)
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

  glAttachShader(hProgram, pVS->m_glHandle);
  glAttachShader(hProgram, pFS->m_glHandle);

  // Link the program
  // ================

  glLinkProgram(hProgram);

  // Get the link status
  // ===================

  GLint status;
  glGetProgramiv(hProgram, GL_LINK_STATUS, &status);
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
  glGetProgramiv(hProgram, GL_INFO_LOG_LENGTH, &messageLength);

  // Get log message
  // ===============

  ezStringBuilder buffer;
  buffer.Reserve((ezUInt32)messageLength - 1);

  auto message = const_cast<GLchar*>(buffer.GetData());
  glGetProgramInfoLog(hProgram, messageLength, &messageLength, message);

  // Forward log message to ezLog
  // ============================

  ezLog::Warning("%s", message);

  // Release the allocated object
  // ============================

  glDeleteProgram(hProgram);

  return nullptr;
}

kr::ShaderProgram::~ShaderProgram()
{
  glDeleteProgram(m_glHandle);
  m_glHandle = 0;
}

//kr::ShaderAttribute kr::getAttribute(ShaderProgram program, const char* attrName)
//{
//  EZ_ASSERT(isValid(program), "Invalid program.");
//  auto location = glGetAttribLocation(program.glHandle, attrName);
//
//  if (location == 0)
//    return ShaderAttribute();
//
//  ShaderAttribute sa;
//  sa.glHandle = location;
//  sa.program = program;
//  return sa;
//}
