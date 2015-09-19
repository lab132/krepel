#include <krEngine/pch.h>
#include <krEngine/rendering/window.h>
#include <krEngine/referenceCounting.h>

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

void kr::Window::setClearColor(const ezColor& color)
{
  getImpl(this).m_clearColor = color;
}

ezColor kr::Window::getClearColor() const
{
  return getImpl(this).m_clearColor;
}

kr::RefCountedPtr<kr::Window> kr::Window::open(ezWindowCreationDesc& desc)
{
  auto pImpl = EZ_DEFAULT_NEW(WindowImpl);

  // Try to actually open the winow.
  if (pImpl->m_handler.Initialize(desc).Failed())
  {
    EZ_DEFAULT_DELETE(pImpl);
    return nullptr;
  }

  // Try to create a rendering context.
  if (createOpenGLContext(*pImpl).Failed())
  {
    EZ_DEFAULT_DELETE(pImpl);
    return nullptr;
  }

  // At this point, everything succeeded.
  return pImpl;
}

void kr::Window::release(Window*& pWindow)
{
  auto pImpl = &getImpl(pWindow);
  pWindow = nullptr;
  EZ_DEFAULT_DELETE(pImpl);
}

ezResult kr::close(RefCountedPtr<Window> pWindow)
{
  // Closing a window that does not exist is a success.
  if (isNull(pWindow))
    return EZ_SUCCESS;
  return internalClose(getImpl(pWindow));
}

void kr::processWindowMessages(RefCountedPtr<Window> pWindow)
{
  if (isNull(pWindow))
  {
    ezLog::Warning("Requesting processing of window messages on an invalid window pointer.");
    return;
  }

  getImpl(pWindow).m_handler.ProcessWindowMessages();
}
