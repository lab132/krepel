#pragma once
#include <krEngine/referenceCounting.h>

#include <System/Window/Window.h>
#include <Foundation/Communication/Event.h>

namespace kr
{
  struct WindowEventArgs
  {
    enum Type
    {
      FocusChange,
      ClickClose,
    };

    Type type;
    bool hasFocusNow;
  };

  using WindowEvent = ezEvent<const WindowEventArgs&>;

  /// \note It's ok to pass instances of this by value.
  class Window : public RefCounted
  {
  public: // *** Static API
    /// \brief How to clean up window objects that are no longer referenced.
    using ReleasePolicy = Window;

    KR_ENGINE_API static void release(Window*& pWindow);

    /// \brief Opens a window with a valid rendering context.
    KR_ENGINE_API static RefCountedPtr<Window> open(ezWindowCreationDesc& desc = ezWindowCreationDesc());

  public: // *** Public Interface
    KR_ENGINE_API WindowEvent& getEvent();
    const WindowEvent& getEvent() const
    {
      return const_cast<Window*>(this)->getEvent();
    }

    KR_ENGINE_API ezSizeU32 getClientAreaSize() const;

    KR_ENGINE_API void setClearColor(const ezColor& color);
    KR_ENGINE_API ezColor getClearColor() const;

  protected: // *** Type Constraints
    Window() = default;                      ///< Default constructor.
    Window(const Window&) = delete;          ///< No copying.
    void operator =(const Window&) = delete; ///< No copy assignment.
  };

  /// \note You do not have to call this yourself. A window closes itself when destroyed.
  /// \note move() your instance to destroy it at the same time as closing it.
  /// \example close(move(m_pWindow)); // Destroys the window after it was closed.
  KR_ENGINE_API ezResult close(RefCountedPtr<Window> pWindow);

  /// \brief Processes messages for the given \a pWindow.
  ///
  /// This is necessary in order to stay responsive and should be done in the main/render loop.
  KR_ENGINE_API void processWindowMessages(RefCountedPtr<Window> pWindow);
}
