#include <krEngine/game/defaultMainModule.h>
#include <krEngine/game/gameLoop.h>
#include <krEngine/common/utils.h>
#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/window.h>

#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>


void kr::DefaultWindow::OnClickCloseMessage()
{
  m_userRequestsClose = true;
}

kr::DefaultMainModule::DefaultMainModule()
{
  ezClock::SetNumGlobalClocks(ezGlobalClockCount);
  ezFileSystem::RegisterDataDirectoryFactory(ezDataDirectory::FolderType::Factory);
}

kr::DefaultMainModule::~DefaultMainModule()
{
}

void kr::DefaultMainModule::OnCoreStartup()
{
  // The directory right above the executable belongs to the empty group.
  if(ezFileSystem::AddDataDirectory(kr::defaultRoot(), ezFileSystem::ReadOnly).Failed())
  {
    ezLog::Error("Failed to mount the default root as read-only data directory.");
  }

  // The current working directory belongs to the empty group "".
  if(ezFileSystem::AddDataDirectory(kr::cwd().GetData(), ezFileSystem::AllowWrites).Failed())
  {
    ezLog::Error("Failed to mount current working directory as writable data directory.");
  }

  if (m_windowDesc.m_Title.IsEmpty())
  {
    m_windowDesc.m_Title = m_plugin.GetPluginName();
  }

  m_htmlLog.BeginLog(ezStringBuilder{ m_plugin.GetPluginName(), "Log.html" }, m_windowDesc.m_Title);
  ezGlobalLog::AddLogWriter({ &ezLogWriter::HTML::LogMessageHandler, &m_htmlLog });
}

void kr::DefaultMainModule::OnEngineStartup()
{
  m_pWindow = Window::createAndOpen(m_windowDesc);
  EZ_VERIFY(m_pWindow != nullptr, "Failed to open window.");

  m_pWindow->getEvent().AddEventHandler([](const WindowEventArgs& e)
  {
    auto userRequestsClose{ e.type == WindowEventArgs::ClickClose };
    GlobalGameLoopRegistry::setKeepTicking(!userRequestsClose);
  });

  // We own this game loop, so the following call should never fail.
  EZ_VERIFY(m_moduleLoop.addCallback("main", { &DefaultMainModule::tick, this }).Succeeded(), "Failed to register module tick function.");

  // The following call may fail if the user supplied their own "module" game loop.
  if(GlobalGameLoopRegistry::add("module", &m_moduleLoop, ezGlobalLog::GetInstance()).Failed())
  {
    ezLog::Error("Failed to register the main module's game loop. Things will probably not work as intended.");
  }
}

void kr::DefaultMainModule::OnEngineShutdown()
{
  GlobalGameLoopRegistry::remove("module", ezGlobalLog::GetInstance());

  if(m_pWindow->close().Failed())
  {
    ezLog::SeriousWarning("Failed to destroy window.");
  }
}

void kr::DefaultMainModule::OnCoreShutdown()
{
  ezGlobalLog::RemoveLogWriter({ &ezLogWriter::HTML::LogMessageHandler, &m_htmlLog });
  m_htmlLog.EndLog();
  ezFileSystem::RemoveDataDirectoryGroup("");
}

void kr::DefaultMainModule::tick()
{
  clock()->Update();
  auto dt = clock()->GetTimeDiff();
  // So far, the delta time is not used here.
  EZ_IGNORE_UNUSED(dt);

  processWindowMessages(m_pWindow);

  kr::Renderer::extract();
  kr::Renderer::update(m_pWindow);
}

ezClock* kr::DefaultMainModule::clock()
{
  return ezClock::Get(ezGlobalClock_GameLogic);
}
