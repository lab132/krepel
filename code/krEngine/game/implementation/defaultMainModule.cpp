#include <krEngine/game/defaultMainModule.h>
#include <krEngine/game/gameLoop.h>

#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>


kr::DefaultMainModule::DefaultMainModule()
{
}

kr::DefaultMainModule::~DefaultMainModule()
{
}

void kr::DefaultMainModule::OnCoreStartup()
{
}

void kr::DefaultMainModule::OnEngineStartup()
{
  EZ_VERIFY(m_window.Initialize().Succeeded(), "Failed to open window");

  // We own this game loop, so the following call should never fail.
  EZ_VERIFY(m_moduleLoop.addCallback("main", { &DefaultMainModule::tick, this }).Succeeded(), "Failed to register module tick function.");

  // The following call may fail if the user supplied their own "module" game loop.
  if (GlobalGameLoopRegistry::add("module", &m_moduleLoop, ezGlobalLog::GetInstance()).Failed())
  {
    ezLog::Error("Failed to register the main module's game loop. Things will probably not work as intended.");
  }
}

void kr::DefaultMainModule::OnEngineShutdown()
{
  GlobalGameLoopRegistry::remove("module", ezGlobalLog::GetInstance());

  if(m_window.Destroy().Failed())
  {
    ezLog::SeriousWarning("Failed to destroy window.");
  }
}

void kr::DefaultMainModule::OnCoreShutdown()
{
}

void kr::DefaultMainModule::tick()
{
  m_window.ProcessWindowMessages();

  if(m_window.userRequestsClose())
  {
    GlobalGameLoopRegistry::setKeepTicking(false);
    return;
  }
}

void kr::DefaultWindow::OnClickCloseMessage()
{
  m_userRequestsClose = true;
}

void kr::DefaultWindow::SetCreationDescription(ezWindowCreationDesc& desc)
{
  if(IsInitialized())
  {
    ezLog::Warning("Setting window creation description after the window has been created does not take effect until the window is re-initialized!");
  }
  m_CreationDescription = desc;
}
