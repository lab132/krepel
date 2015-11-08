#pragma once

#include <krEngine/game/mainModule.h>
#include <krEngine/game/gameLoop.h>

#include <System/Window/Window.h>
#include <Foundation/Logging/HTMLWriter.h>

namespace kr
{
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
    DefaultWindow* window() { return &m_window; }

  protected: // *** Data
    ezPlugin m_plugin{ false };
    ezWindowCreationDesc m_windowDesc;
    DefaultWindow m_window;
    GameLoop m_moduleLoop;
    ezLogWriter::HTML m_htmlLog;
  };
}
