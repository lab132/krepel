#pragma once
#include <krEngine/ownership.h>
#include <krEngine/rendering/texture.h>

namespace kr
{
  class VertexShader
  {
  public: // *** Data
    /// \note You should not fiddle around with this directly.
    GLuint m_glHandle = 0;
    ezString128 m_resourceId;

  public: // *** Static API
    /// \brief Loads and compiles a vertex shader from the source code located in \a filename.
    KR_ENGINE_API static Owned<VertexShader> loadAndCompile(ezStringView filename);

  public: // *** Construction
    KR_ENGINE_API ~VertexShader();

  public: // *** Accessors/Mutators
    ezUInt32 getGlHandle() const { return m_glHandle; }

  private: // *** Private Construction
    /// \brief Can only be constructed by loadAndCompile.
    VertexShader() = default;
    VertexShader(const VertexShader&) = delete;
    void operator =(const VertexShader&) = delete;
  };


  class FragmentShader
  {
  public: // *** Data
    /// \note You should not fiddle around with this directly.
    GLuint m_glHandle = 0;
    ezString128 m_resourceId;

  public: // *** Static API
    /// \brief Loads and compiles a vertex shader from the source code located in \a filename.
    KR_ENGINE_API static Owned<FragmentShader> loadAndCompile(ezStringView filename);

  public: // *** Construction
    KR_ENGINE_API ~FragmentShader();

  public: // *** Accessors/Mutators
    ezUInt32 getGlHandle() const { return m_glHandle; }

  private: // *** Private Construction
    /// \brief Can only be constructed by loadAndCompile.
    FragmentShader() = default;
    FragmentShader(const FragmentShader&) = delete;
    void operator =(const FragmentShader&) = delete;
  };


  class ShaderProgram
  {
  public: // *** Static API
    /// \brief Links the given vertex and fragment shaders \a pVS and \a pFS to a program.
    KR_ENGINE_API static Owned<ShaderProgram> link(Borrowed<VertexShader> vs,
                                                   Borrowed<FragmentShader> fs);
    static Owned<ShaderProgram> loadAndLink(ezStringView vsFileName,
                                                    ezStringView fsFileName)
    {
      auto vs = VertexShader::loadAndCompile(vsFileName);
      auto fs = FragmentShader::loadAndCompile(fsFileName);
      return link(borrow(vs), borrow(fs));
    }

  public: // *** Data
    /// \note You should not fiddle around with this directly.
    ezUInt32 m_glHandle = 0;

  public: // *** Construction
    KR_ENGINE_API ~ShaderProgram();

  public: // *** Accessors/Mutators
    ezUInt32 getGlHandle() const { return m_glHandle; }

  private: // *** Private Construction
    /// \brief Can only be constructed by link.
    ShaderProgram() = default;

    EZ_DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
  };


  struct ShaderUniform
  {
    GLuint glLocation = -1;
    Borrowed<ShaderProgram> pShader;
  };

  KR_ENGINE_API ShaderUniform shaderUniformOf(Borrowed<ShaderProgram> pShader,
                                              ezStringView uniformName);

  /// \brief Uploads an \a ezColor value.
  KR_ENGINE_API ezResult uploadData(const ShaderUniform& uniform,
                                    ezColor value);

  /// \brief Uploads a texture slot.
  KR_ENGINE_API ezResult uploadData(const ShaderUniform& uniform,
                                    TextureSlot slot);

  KR_ENGINE_API ezResult uploadData(const ShaderUniform& uniform,
                                    const ezMat4& matrix);

  KR_ENGINE_API ezResult uploadData(const ShaderUniform& uniform,
                                    const ezVec2& matrix);

  KR_ENGINE_API ezResult uploadData(const ShaderUniform& uniform,
                                    const ezAngle& matrix);

  KR_ENGINE_API ezResult bind(Borrowed<const ShaderProgram> pShader);

  /// \see KR_RAII_BIND_SHADER
  KR_ENGINE_API ezResult restoreLastShaderProgram();
}

/// \brief Binds the given shader for the current scope.
#define KR_RAII_BIND_SHADER(shader) \
  ::kr::bind(shader);               \
  KR_ON_SCOPE_EXIT { ::kr::restoreLastShaderProgram(); }
