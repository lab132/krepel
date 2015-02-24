#pragma once
#include <krEngine/rendering/window.h>

namespace kr
{
  class WindowHandler : public ezWindow
  {
  public: // *** Construction
    WindowHandler(WindowEvent& event) : m_Event(event) {}

  public: // *** Overrides
    virtual void OnFocusMessage(bool bHasFocus) override
    {
      WindowEventArgs args;
      args.type = WindowEventArgs::FocusChange;
      args.hasFocusNow = bHasFocus;
      m_Event.Broadcast(args);
    }

    virtual void OnClickCloseMessage() override
    {
      WindowEventArgs args;
      args.type = WindowEventArgs::ClickClose;
      m_Event.Broadcast(args);
    }

  private: // *** Data
    WindowEvent& m_Event;
  };

  class WindowImpl : public Window
  {
  public:
    HDC m_hDC = nullptr;
    HGLRC m_hRC = nullptr;
    WindowEvent m_Event;
    ezColor m_clearColor = ezColor::GetBlack();
    WindowHandler m_handler;

    WindowImpl() : m_handler(m_Event) {}
    ~WindowImpl();
  };

  inline kr::WindowImpl& getImpl(kr::Window* pWindow)
  {
    return *static_cast<kr::WindowImpl*>(pWindow);
  }

  inline const kr::WindowImpl& getImpl(const kr::Window* pWindow)
  {
    return *static_cast<const kr::WindowImpl*>(pWindow);
  }
}
