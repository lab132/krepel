#pragma once
#include <krEngine/ownership.h>
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

  enum class PrimitiveType
  {
    Points = GL_POINTS,

    LineStrip = GL_LINE_STRIP,
    LineLoop = GL_LINE_LOOP,
    Lines = GL_LINES,
    LineStripAdjacency = GL_LINE_STRIP_ADJACENCY,
    LinesAdjacency = GL_LINES_ADJACENCY,

    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan = GL_TRIANGLE_FAN,
    Triangles = GL_TRIANGLES,
    TriangleStripAdjacency = GL_TRIANGLE_STRIP_ADJACENCY,
    TrianglesAdjacency = GL_TRIANGLES_ADJACENCY,

    Patches = GL_PATCHES
  };

  class VertexBuffer
  {
  public: // *** Static API
    KR_ENGINE_API static Owned<VertexBuffer> create(BufferUsage usage,
                                                    PrimitiveType primitive);

  public: // *** Types
    struct VertexArrayProgramPair
    {
      GLuint hVao = 0;
      ShaderProgramPtr pShader;
    };

  public: // *** Data
    BufferUsage m_usage;       ///< Set on construction.
    PrimitiveType m_primitive; ///< Set on construction.
    BufferTarget m_target = BufferTarget::Array;

    GLuint m_glHandle = 0;
    ezHybridArray<VertexArrayProgramPair, 1> m_Vaos;

  public: // *** Accessors/Mutators
    void setUsage(BufferUsage usage) { m_usage = usage; }
    BufferUsage getUsage() const { return m_usage; }

    void setPrimitive(PrimitiveType primitive) { m_primitive = primitive; }
    PrimitiveType getPrimitive() const { return m_primitive; }

    void setTarget(BufferTarget target) { m_target = target; }
    BufferTarget getTarget() const { return m_target; }

  public: // *** Construction
    KR_ENGINE_API ~VertexBuffer();

  private: // *** Private Construction
    /// Create these using a static factory function of this class.
    VertexBuffer() = default;

    VertexBuffer(const VertexBuffer&) = delete;
    void operator =(const VertexBuffer&) = delete;
  };

  /// \brief Sets the layout of this vertex buffer for the given \a pShader.
  /// \param layoutTypeName
  ///   The name of a reflectable (ezRTTI) type descibing the
  ///   vertex buffer layout. Yes, this is Black Magic™.
  KR_ENGINE_API ezResult setupLayout(kr::Borrowed<VertexBuffer> pVertBuffer,
                                     ShaderProgramPtr pShader,
                                     const char* layoutTypeName);

  KR_ENGINE_API ezResult uploadData(kr::Borrowed<const VertexBuffer> pVertBuffer,
                                    ezUInt32 byteCount,
                                    const void* bytes,
                                    ezUInt32 offet = 0);

  template<typename T>
  ezResult uploadData(kr::Borrowed<const VertexBuffer> pVertBuffer,
                      ezArrayPtr<T> data,
                      ezUInt32 offset = 0)
  {
    return uploadData(forward<decltype(pVertBuffer)>(pVertBuffer),
                      data.GetCount() * sizeof(T), // byteCount
                      (const void*)data.GetPtr(),  // bytes
                      offset);                     // offset
  }

  KR_ENGINE_API ezResult bind(kr::Borrowed<const VertexBuffer> pVertBuffer, ShaderProgramPtr pProgram);

  KR_ENGINE_API ezResult restoreLastVertexBuffer(ShaderProgramPtr pProgram);
}

/// \brief Binds the given vertex buffer for the given shader program for the current scope.
#define KR_RAII_BIND_VERTEX_BUFFER(vb, prg) \
  ::kr::bind(vb, prg);                      \
  KR_ON_SCOPE_EXIT { ::kr::restoreLastVertexBuffer(prg); }
