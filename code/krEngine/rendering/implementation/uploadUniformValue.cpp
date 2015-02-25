#include <krEngine/rendering/shader.h>

#define CHECK_PRECONDITIONS(pShader, uniform)      \
  do                                               \
  {                                                \
    if (isNull(pShader))                           \
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


ezResult kr::uploadUniformValue(RefCountedPtr<ShaderProgram> pShader,
                                ShaderUniform uniform,
                                ezColor value)
{
  CHECK_PRECONDITIONS(pShader, uniform);

  glProgramUniform4fv(pShader->m_glHandle,
                      uniform.glLocation,
                      1, value.GetData());

  return EZ_SUCCESS;
}

ezResult kr::uploadUniformValue(RefCountedPtr<ShaderProgram> pShader,
                                ShaderUniform uniform,
                                RefCountedPtr<Texture> pTex)
{
  CHECK_PRECONDITIONS(pShader, uniform);

  glProgramUniform1i(pShader->m_glHandle,
                     uniform.glLocation,
                     pTex->getSlot());

  return EZ_SUCCESS;
}


#undef CHECK_PRECONDITIONS
