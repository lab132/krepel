#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/window.h>
#include <krEngine/rendering/implementation/windowImpl.h>

#include <krEngine/rendering/implementation/extractionBuffer.h>

namespace
{
  using ExtractionAllocator = ezAllocator<ezMemoryPolicies::ezAlignedHeapAllocation, ezMemoryTrackingFlags::All>;
}

// Globals
// =======
static bool g_initialized = false;
static kr::ExtractionBuffer* g_pReadBuffer;
static kr::ExtractionBuffer* g_pWriteBuffer;

void GLAPIENTRY debugCallbackOpenGL(GLenum source,
                                    GLenum type,
                                    GLuint id,
                                    GLenum severity,
                                    GLsizei length,
                                    const GLchar* message,
                                    const void* userParam)
{
  /// \todo Implement me.
}

// clang-format off
namespace kr
{
  EZ_BEGIN_SUBSYSTEM_DECLARATION(SpriteEngine, Renderer)
    BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
    END_SUBSYSTEM_DEPENDENCIES

    SpriteEngineRendererSubSystem() :
      m_extractionAllocator("Extraction")
    {
    }

    ExtractionAllocator m_extractionAllocator;

    char m_readBufferMemory[sizeof(ExtractionBuffer)];
    char m_writeBufferMemory[sizeof(ExtractionBuffer)];

    ON_CORE_STARTUP
    {
      g_pReadBuffer = new (m_readBufferMemory) ExtractionBuffer(&m_extractionAllocator);
      g_pReadBuffer->setMode(ExtractionBuffer::Mode::ReadOnly);

      g_pWriteBuffer = new (m_writeBufferMemory) ExtractionBuffer(&m_extractionAllocator);
      g_pReadBuffer->setMode(ExtractionBuffer::Mode::WriteOnly);

      g_initialized = true;
    }

    ON_ENGINE_STARTUP
    {
      //glDebugMessageCallback(debugCallbackOpenGL, nullptr);
    }

    ON_ENGINE_SHUTDOWN
    {

      //glDebugMessageCallback(nullptr, nullptr);
    }

    ON_CORE_SHUTDOWN
    {
      g_pWriteBuffer->~ExtractionBuffer();
      g_pWriteBuffer = nullptr;

      g_pReadBuffer->~ExtractionBuffer();
      g_pReadBuffer = nullptr;

      g_initialized = false;
    }
  EZ_END_SUBSYSTEM_DECLARATION
}
// clang-format on


void kr::Renderer::extract()
{
  swap(g_pReadBuffer, g_pWriteBuffer);
}

static ezResult presentFrame(const kr::WindowImpl& window)
{
  return ::SwapBuffers(window.m_hDC) ? EZ_SUCCESS
                                     : EZ_FAILURE;
}

void kr::Renderer::update(ezTime dt, RefCountedPtr<Window> pTarget)
{
  if(isNull(pTarget))
  {
    ezLog::Warning("Invalid target window.");
    return;
  }

  auto& window = getImpl(pTarget);
  /// \todo This is Window specific.
  wglMakeCurrent(window.m_hDC, window.m_hRC);

  {
    auto& clearColor = window.m_clearColor;
    glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /// \todo Do the actual rendering here.

  if (presentFrame(window).Failed())
  {
    ezLog::Warning("Failed to present frame.");
  }
}
