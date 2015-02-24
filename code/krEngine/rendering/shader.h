#pragma once
#include <krEngine/referenceCounting.h>

namespace kr
{
  class VertexShader : public RefCounted
  {
  public: // *** Data
    /// \note You should not fiddle around with this directly.
    GLuint m_glHandle = 0;

  public: // *** Static API
    using ReleasePolicy = RefCountedReleasePolicies::DefaultDelete;

    /// \brief Loads and compiles a vertex shader from the source code located in \a filename.
    KR_ENGINE_API static RefCountedPtr<VertexShader> loadAndCompile(const char* filename);

  public: // *** Construction
    KR_ENGINE_API ~VertexShader();

  private: // *** Private Construction
    /// \brief Can only be constructed by loadAndCompile.
    VertexShader() = default;

    EZ_DISALLOW_COPY_AND_ASSIGN(VertexShader);
  };

  class FragmentShader : public RefCounted
  {
  public: // *** Data
    /// \note You should not fiddle around with this directly.
    GLuint m_glHandle = 0;

  public: // *** Static API
    using ReleasePolicy = RefCountedReleasePolicies::DefaultDelete;

    /// \brief Loads and compiles a vertex shader from the source code located in \a filename.
    KR_ENGINE_API static RefCountedPtr<FragmentShader> loadAndCompile(const char* filename);

  public: // *** Construction
    KR_ENGINE_API ~FragmentShader();

  private: // *** Private Construction
    /// \brief Can only be constructed by loadAndCompile.
    FragmentShader() = default;

    EZ_DISALLOW_COPY_AND_ASSIGN(FragmentShader);
  };

  class ShaderProgram : public RefCounted
  {
  public: // *** Static API
    using ReleasePolicy = RefCountedReleasePolicies::DefaultDelete;

    /// \brief Links the given vertex and fragment shaders \a pVS and \a pFS to a program.
    KR_ENGINE_API
    static RefCountedPtr<ShaderProgram> link(RefCountedPtr<VertexShader> pVS,
                                             RefCountedPtr<FragmentShader> pFS);
  public: // *** Data
    /// \note You should not fiddle around with this directly.
    GLuint m_glHandle = 0;

  public: // *** Construction
    KR_ENGINE_API ~ShaderProgram();

  private: // *** Private Construction
    /// \brief Can only be constructed by link.
    ShaderProgram() = default;

    EZ_DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
  };
}
