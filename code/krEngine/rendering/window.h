#pragma once
#include <krEngine/ownership/owned.h>
#include <krEngine/ownership/borrowed.h>

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

  class KR_ENGINE_API Window
  {
  public: // *** Static API
    static Owned<Window> create(ezWindowCreationDesc& desc = ezWindowCreationDesc());

    static Owned<Window> createAndOpen(ezWindowCreationDesc& desc = ezWindowCreationDesc())
    {
      auto w = create(desc);
      if (w->open().Failed())
      {
        return nullptr;
      }
      return move(w);
    }

  public: // *** Initialization
    virtual ~Window() {}

  public: // *** Runtime

    ezResult open();
    ezResult close();

  public: // *** Accessors/Mutators

    WindowEvent& getEvent();

    ezSizeU32 getClientAreaSize() const;

  protected: // *** Internal
    Window() = default;
    Window(const Window&) = delete;          // No copying.
    void operator =(const Window&) = delete; // No copy assignment.
  };

  /// \brief Processes messages for the given \a pWindow.
  ///
  /// This is necessary in order to stay responsive and should be done in the main/render loop.
  KR_ENGINE_API void processWindowMessages(Borrowed<Window> window);
}
