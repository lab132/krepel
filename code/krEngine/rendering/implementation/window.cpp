#include <krEngine/rendering/window.h>
#include <krEngine/ownership.h>

#include "windowImpl.h"

static ezResult destroyOpenGLContext(kr::WindowImpl& window);
static ezResult createOpenGLContext(kr::WindowImpl& window);
// The following include implements the two functions above.
#include "openGlContext.inl"

static ezResult internalClose(kr::WindowImpl& window)
{
  using namespace kr;

  if (window.m_hRC || window.m_hDC)
  {
    /// \todo Should we check for success/failure of this call?
    destroyOpenGLContext(window);
  }
  return window.m_handler.Destroy();
}

kr::WindowImpl::~WindowImpl()
{
  internalClose(*this);
}

kr::WindowEvent& kr::Window::getEvent()
{
  return getImpl(this).m_Event;
}

ezSizeU32 kr::Window::getClientAreaSize() const
{
  return getImpl(this).m_handler.GetClientAreaSize();
}

ezResult kr::Window::open()
{
  return createOpenGLContext(getImpl(this));
}

kr::Owned<kr::Window> kr::Window::create(ezWindowCreationDesc& desc)
{
  WindowImpl* pImpl = EZ_DEFAULT_NEW(WindowImpl);

  // Try to actually open the winow.
  if (pImpl->m_handler.Initialize(desc).Failed())
  {
    EZ_DEFAULT_DELETE(pImpl);
    return nullptr;
  }

  return own<Window>(pImpl, [](Window* ptr) { EZ_DEFAULT_DELETE(ptr); });
}

ezResult kr::Window::close()
{
  return internalClose(getImpl(this));
}

void kr::processWindowMessages(Borrowed<Window> window)
{
  if (window == nullptr)
  {
    ezLog::Warning("Processing of window messages on an invalid window pointer.");
    return;
  }

  getImpl(window).m_handler.ProcessWindowMessages();
}
