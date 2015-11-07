#include <krEngine/game/defaultMainModule.h>

#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>


kr::DefaultMainModule* kr::DefaultMainModule::instance()
{
  return static_cast<decltype(this)>(kr::MainModule::instance());
}

void kr::DefaultMainModule::startupCore()
{
  ezStartup::StartupCore();
}

void kr::DefaultMainModule::startupEngine()
{
  EZ_ASSERT_ALWAYS(m_window.Initialize().Succeeded(), "Failed to open window");

  ezStartup::StartupEngine();
}

void kr::DefaultMainModule::shutdownEngine()
{
  ezStartup::ShutdownEngine();

  if (m_window.Destroy().Failed())
  {
    ezLog::SeriousWarning("Failed to destroy window.");
  }
}

void kr::DefaultMainModule::shutdownCore()
{
  ezStartup::ShutdownCore();
}

void kr::DefaultMainModule::tick()
{
  // TODO
}

void kr::DefaultWindow::OnClickCloseMessage()
{
  m_userRequestsClose = true;
}

void kr::DefaultWindow::SetCreationDescription(ezWindowCreationDesc& desc)
{
  if (IsInitialized())
  {
    ezLog::Warning("Setting window creation description after the window has been created does not take effect until the window is re-initialized!");
  }
  m_CreationDescription = desc;
}
