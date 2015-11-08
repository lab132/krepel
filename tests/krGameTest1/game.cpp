#include <krEngine/game.h>

class krGameTest1 : public kr::DefaultMainModule
{
  virtual void OnCoreStartup() override
  {
    ezWindowCreationDesc desc{ window()->GetCreationDescription() };
    desc.m_Title = "Game Test 1";
    window()->SetCreationDescription(desc);

    kr::DefaultMainModule::OnCoreStartup();
  }
} static g_mainModule; // <-- A static instance is mandatory!
