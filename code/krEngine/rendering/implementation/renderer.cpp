#include <krEngine/pch.h>
#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/extraction.h>
#include <krEngine/rendering/window.h>

#include <krEngine/rendering/implementation/windowImpl.h>
#include <krEngine/rendering/implementation/opelGlCheck.h>
#include <krEngine/rendering/implementation/extractionBuffer.h>
#include <krEngine/rendering/implementation/extractionDetails.h>

#include <CoreUtils/Graphics/Camera.h>

namespace
{
  using ExtractionAllocator = ezAllocator<ezMemoryPolicies::ezAlignedHeapAllocation,
                                          ezMemoryTrackingFlags::All>;

}

// Globals
// =======
static bool g_initialized = false;
static ezLogInterface* g_pLog = nullptr;

static kr::ExtractionBuffer* g_pReadBuffer;
static kr::ExtractionBuffer* g_pWriteBuffer;
static kr::Renderer::ExtractionEvent g_ExtractionEvent;

static bool g_isCameraSetForCurrentFrame = false;
static ezMat4 g_view;
static ezMat4 g_projection;

static void GLAPIENTRY debugCallbackOpenGL(GLenum source,
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
  EZ_BEGIN_SUBSYSTEM_DECLARATION(krEngine, Renderer)
    BEGIN_SUBSYSTEM_DEPENDENCIES
    "Foundation",
    "Core"
    END_SUBSYSTEM_DEPENDENCIES

    krEngineRendererSubSystem() :
      m_extractionAllocator("Extraction")
    {
    }

    ExtractionAllocator m_extractionAllocator;

    ezByte m_mem_readBuffer[sizeof(ExtractionBuffer)];
    ezByte m_mem_writeBuffer[sizeof(ExtractionBuffer)];

    ON_CORE_STARTUP
    {
      g_pReadBuffer = new (m_mem_readBuffer) ExtractionBuffer(&m_extractionAllocator);
      g_pReadBuffer->setMode(ExtractionBuffer::Mode::ReadOnly);

      g_pWriteBuffer = new (m_mem_writeBuffer) ExtractionBuffer(&m_extractionAllocator);
      g_pReadBuffer->setMode(ExtractionBuffer::Mode::WriteOnly);

      g_view.SetIdentity();
      g_projection.SetIdentity();

      g_initialized = true;
    }

    ON_ENGINE_STARTUP
    {
      g_pLog = ezGlobalLog::GetInstance();
      //glDebugMessageCallback(debugCallbackOpenGL, nullptr);
    }

    ON_ENGINE_SHUTDOWN
    {
      g_pLog = nullptr;
      //glDebugMessageCallback(nullptr, nullptr);
    }

    ON_CORE_SHUTDOWN
    {
      g_projection.SetIdentity();
      g_view.SetIdentity();

      g_pWriteBuffer->~ExtractionBuffer();
      g_pWriteBuffer = nullptr;

      g_pReadBuffer->~ExtractionBuffer();
      g_pReadBuffer = nullptr;

      g_initialized = false;
    }
  EZ_END_SUBSYSTEM_DECLARATION
}
// clang-format on

namespace kr
{
  namespace Renderer
  {
    class ExtractorImpl : public Extractor
    {
    public:
      ExtractorImpl() = default;
    };
  }
}

static void renderExtractionData(ezUByte* current, ezUByte* max)
{
  using namespace kr;

  while(current < max)
  {
    // Only special `ExtractionData` can be allocated with our buffers.
    auto data = reinterpret_cast<ExtractionData*>(current);

    // See what type the data is,
    // then call a specialized function that processes the data.
    switch(data->type)
    {
    case ExtractionDataType::Sprite:
    {
      auto& sprite = *static_cast<SpriteData*>(data);
      draw(sprite, g_view, g_projection);
      sprite.~SpriteData();
    }
      break;
    default:
      EZ_REPORT_FAILURE("Unknown extraction data type.");
      break;
    }

    EZ_ASSERT(current + data->byteCount <= max, "Must never exceed max!");
    current += data->byteCount;
  }
}

static ezResult presentFrame(const kr::WindowImpl& window)
{
  return ::SwapBuffers(window.m_hDC) ? EZ_SUCCESS
                                     : EZ_FAILURE;
}

void kr::Renderer::extract()
{
  g_isCameraSetForCurrentFrame = false;

  // Reset the write buffer's allocation pointer.
  g_pWriteBuffer->reset();

  // Broadcast the extraction event.
  ExtractorImpl e;
  g_ExtractionEvent.Broadcast(e);

  // Swap read/write buffers.
  swap(g_pReadBuffer, g_pWriteBuffer);

  // If no camera was set during the extraction event, emit a warning.
  if (!g_isCameraSetForCurrentFrame)
  {
    ezLog::Warning(g_pLog, "No camera set for current frame.");
  }
}

void kr::Renderer::update(ezTime dt, RefCountedPtr<Window> pTarget)
{
  if (isNull(pTarget))
  {
    ezLog::Warning(g_pLog, "Invalid target window.");
    return;
  }

  auto& window = getImpl(pTarget);
  /// \todo This is Window specific.
  glCheck(wglMakeCurrent(window.m_hDC, window.m_hRC));

  // Clear the Screen
  // ================
  {
    auto& clearColor = window.m_clearColor;
    glCheck(glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a));
  }

  glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

  // Render the Data
  // ===============
  {
    ezUByte* current = begin(*g_pReadBuffer);
    ezUByte* max = end(*g_pReadBuffer);
    renderExtractionData(current, max);
  }

  // Swap Buffers
  // ============

  if (presentFrame(window).Failed())
  {
    ezLog::Warning(g_pLog, "Failed to present frame.");
  }
}

void kr::Renderer::addExtractionListener(ExtractionEventListener listener)
{
  g_ExtractionEvent.AddEventHandler(listener);
}

void kr::Renderer::removeExtractionListener(ExtractionEventListener listener)
{
  g_ExtractionEvent.RemoveEventHandler(listener);
}

// Extraction
// ==========

void kr::extract(Renderer::Extractor& e, const ezCamera& cam, float aspectRatio)
{
  cam.GetViewMatrix(g_view);
  cam.GetProjectionMatrix(aspectRatio,                           // Aspect Ratio, i.e. width / height
                          ezProjectionDepthRange::MinusOneToOne, // The GL-Way.
                          g_projection);                         // [out] Projection matrix.
  g_isCameraSetForCurrentFrame = true;
}

void kr::extract(Renderer::Extractor& e,
                 const Sprite& sprite,
                 Transform2D transform)
{
  auto pData = g_pWriteBuffer->allocate<SpriteData>();
  pData->pTexture = sprite.getTexture();
  pData->pVertexBuffer = sprite.getVertexBuffer();
  pData->pShader = sprite.getShader();
  pData->pSampler = sprite.getSampler();

  pData->uColor = sprite.getColorUniform();
  pData->uTexture = sprite.getTextureUniform();
  pData->uOrigin = sprite.getOriginUniform();
  pData->uRotation = sprite.getRotationUniform();
  pData->uViewMatrix = sprite.getViewMatrixUniform();
  pData->uProjectionMatrix = sprite.getProjectionMatrixUniform();

  pData->transform = move(transform);
  pData->color = sprite.getColor();
}

// Drawing
// =======

void kr::draw(SpriteData& sprite,
              const ezMat4& viewMatrix,
              const ezMat4& projectionMatrix)
{
  EZ_LOG_BLOCK("Drawing Sprite");

  // If there is no shader, we cannot draw.
  if (isNull(sprite.pShader))
  {
    ezLog::Warning("No shader to draw with.");
    return;
  }

  TextureSlot textureSlot(0);

  // Set Active Shader
  // =================
  KR_RAII_BIND_SHADER(sprite.pShader);
  KR_RAII_BIND_VERTEX_BUFFER(sprite.pVertexBuffer, sprite.pShader);
  KR_RAII_BIND_SAMPLER(sprite.pSampler, textureSlot);
  KR_RAII_BIND_TEXTURE_2D(sprite.pTexture, textureSlot);

  // Update Uniforms
  // ===============
  uploadData(sprite.uColor, sprite.color);
  uploadData(sprite.uTexture, textureSlot);
  uploadData(sprite.uOrigin, sprite.transform.m_position);
  uploadData(sprite.uRotation, sprite.transform.m_rotation);
  uploadData(sprite.uViewMatrix, viewMatrix);
  uploadData(sprite.uProjectionMatrix, projectionMatrix);

  glCheck(glDrawArrays((GLenum)sprite.pVertexBuffer->getPrimitive(), 0, 4));
}

