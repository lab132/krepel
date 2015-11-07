#pragma once

#include <krEngine/game/mainModule.h>

#include <System/Window/Window.h>

namespace kr
{
  class KR_ENGINE_API DefaultWindow : public ezWindow
  {
  public: // *** Inherited from ezWindow
    virtual void OnClickCloseMessage();

  public: // *** Accessors

    void SetCreationDescription(ezWindowCreationDesc& desc);
    bool userRequestsClose() const { return m_userRequestsClose; }

  protected: // *** Data
    bool m_userRequestsClose;
  };

  class KR_ENGINE_API DefaultMainModule : public MainModule
  {
  public: // *** Static API
    DefaultMainModule* instance();

  public: // *** Inherited via MainModule
    virtual void startupCore() override;
    virtual void startupEngine() override;
    virtual void shutdownEngine() override;
    virtual void shutdownCore() override;
    virtual void tick() override;
    virtual bool keepTicking() const override { return m_keepTicking; }

  public: // *** Accessors
    DefaultWindow* window() { return &m_window; }

  protected: // *** Data
    bool m_keepTicking = true;
    DefaultWindow m_window;
  };
}
