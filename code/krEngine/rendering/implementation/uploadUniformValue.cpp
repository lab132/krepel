#include <krEngine/rendering/shader.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>

#define PRECONDITIONS(uniform, type)               \
  EZ_LOG_BLOCK("Uploading Uniform Value", type);   \
  do                                               \
  {                                                \
    if (isNull(uniform.pShader))                   \
    {                                              \
      ezLog::Warning("Invalid shader object.");    \
      return EZ_FAILURE;                           \
    }                                              \
    if (uniform.glLocation == -1)                  \
    {                                              \
      ezLog::Warning("Invalid uniform location."); \
      return EZ_FAILURE;                           \
    }                                              \
  } while (false)


ezResult kr::uploadData(const ShaderUniform& uniform,
                        ezColor value)
{
  PRECONDITIONS(uniform, "ezColor");

  glCheck(glProgramUniform4fv(uniform.pShader->m_glHandle,
                              uniform.glLocation,
                              1, value.GetData()));

  return EZ_SUCCESS;
}

ezResult kr::uploadData(const ShaderUniform& uniform,
                        TextureSlot slot)
{
  PRECONDITIONS(uniform, "Texture");

  glCheck(glProgramUniform1i(uniform.pShader->m_glHandle,
                             uniform.glLocation,
                             slot.value));

  return EZ_SUCCESS;
}


#undef PRECONDITIONS
