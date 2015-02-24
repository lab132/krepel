#pragma once
#include <krEngine/referenceCounting.h>
#include <krEngine/rendering/shader.h>

namespace kr
{
  enum class BufferUsage
  {
    StaticDraw   = GL_STATIC_DRAW, ///< Modified once by application, used many times for drawing.
    StaticRead   = GL_STATIC_READ, ///< Modified once and read back by the application.
    StaticStream = GL_STATIC_COPY, ///< Modified once by OpenGL, used many times for drawing

    DynamicDraw   = GL_DYNAMIC_DRAW, ///< Modified many times by application, used many times for drawing.
    DynamicRead   = GL_DYNAMIC_READ, ///< Modified many times and read back by the application.
    DynamicStream = GL_DYNAMIC_COPY, ///< Modified many times by OpenGL, used many times for drawing

    StreamDraw   = GL_STREAM_DRAW, ///< Modified once by application, used only a few times for drawing.
    StreamRead   = GL_STREAM_READ, ///< Modified once by and read back the application.
    StreamStream = GL_STREAM_COPY, ///< Modified once by OpenGL, used only a few times for drawing
  };

  enum class BufferTarget
  {
    Array = GL_ARRAY_BUFFER
  };

  class VertexBuffer : public RefCounted
  {
  public: // *** Static API
    using ReleasePolicy = RefCountedReleasePolicies::DefaultDelete;

    KR_ENGINE_API static RefCountedPtr<VertexBuffer> create();

  public: // *** Types
    struct VertexArrayProgramPair
    {
      GLuint hVao = 0;
      RefCountedPtr<ShaderProgram> pProgram;
    };

  public: // *** Data
    BufferUsage m_usage = BufferUsage::StaticDraw;
    BufferTarget m_target = BufferTarget::Array;

    GLuint m_glHandle = 0;
    ezHybridArray<VertexArrayProgramPair, 1> m_Vaos;

  public: // *** Accessors/Mutators
    void setUsage(BufferUsage usage) { m_usage = usage; }
    BufferUsage getUsage() const { return m_usage; }

    void setTarget(BufferTarget target) { m_target = target; }
    BufferTarget getTarget() const { return m_target; }

  public: // *** Construction
    KR_ENGINE_API ~VertexBuffer();

  private: // *** Private Construction
    // Create these using a static factory function of this class.
    VertexBuffer() = default;

    EZ_DISALLOW_COPY_AND_ASSIGN(VertexBuffer);
  };

  /// \brief Sets the layout of this vertex buffer for the given program.
  /// \param layoutTypeName
  ///        The name of a reflectable (ezRTTI) type descibing the
  ///        vertex buffer layout. Yes, this is Black Magic™.
  KR_ENGINE_API ezResult setupLayout(RefCountedPtr<VertexBuffer> pVertBuffer,
                                         RefCountedPtr<ShaderProgram> program,
                                         const char* layoutTypeName);

  KR_ENGINE_API ezResult uploadData(RefCountedPtr<VertexBuffer> pVertBuffer,
                                        ezUInt32 byteCount, const void* bytes,
                                        ezUInt32 offet = 0);

  template<typename T>
  ezResult uploadData(RefCountedPtr<VertexBuffer> pVertBuffer,
                      ezArrayPtr<T> data,
                      ezUInt32 offet)
  {
    return uploadData(forward<decltype(pVertBuffer)>(pVertBuffer),
                      data.GetCount() * sizeof(T), // byteCount
                      (const void*)data.GetPtr(),  // bytes
                      offset);                     // offset
  }

  template<typename T>
  ezResult uploadData(RefCountedPtr<VertexBuffer> pVertBuffer,
                      ezArrayPtr<T> data)
  {
    return uploadData(forward<decltype(pVertBuffer)>(pVertBuffer),
                      data.GetCount() * sizeof(T), // byteCount
                      (const void*)data.GetPtr(),  // bytes
                      0);                          // offset
  }
}
