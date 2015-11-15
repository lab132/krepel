#pragma once

#include <krEngine/ownership.h>
#include <krEngine/game/mainModule.h>
#include <krEngine/game/gameLoop.h>

#include <System/Window/Window.h>
#include <Foundation/Logging/HTMLWriter.h>

class ezClock;

namespace kr
{
  class Window;

  class KR_ENGINE_API DefaultWindow : public ezWindow
  {
  public: // *** Inherited from ezWindow
    virtual void OnClickCloseMessage();

  public: // *** Accessors

    bool userRequestsClose() const { return m_userRequestsClose; }

  protected: // *** Data
    bool m_userRequestsClose;
  };

  class KR_ENGINE_API DefaultMainModule : public MainModule
  {
  public: // *** Construction
    DefaultMainModule();
    virtual ~DefaultMainModule();

  public: // *** Inherited via kr::MainModule
    virtual void OnCoreStartup() override;
    virtual void OnEngineStartup() override;
    virtual void OnEngineShutdown() override;
    virtual void OnCoreShutdown() override;

  public: // *** Runtime
    void tick();

  public: // *** Accessors
    Borrowed<Window> window() { return m_pWindow; }
    ezClock* clock();

  protected: // *** Data
    ezPlugin m_plugin{ false };
    ezWindowCreationDesc m_windowDesc;
    Owned<Window> m_pWindow;
    GameLoop m_moduleLoop;
    ezLogWriter::HTML m_htmlLog;
  };
}
