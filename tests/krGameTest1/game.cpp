#include <krEngine/game.h>

class krGameTest1Subsystem : public ezSubSystem
{
  BEGIN_SUBSYSTEM_DEPENDENCIES
    "krEngine"
  END_SUBSYSTEM_DEPENDENCIES

public:
  virtual const char* GetGroupName() const override { return "krGameTest1"; }
  virtual const char* GetSubSystemName() const override { return "main"; }

  virtual void OnCoreStartup()
  {
    auto pMod{ static_cast<kr::DefaultMainModule*>(kr::MainModule::instance()) };
    ezWindowCreationDesc desc{ pMod->window()->GetCreationDescription() };
    desc.m_Title = "Game Test 1";
    pMod->window()->SetCreationDescription(desc);
  }
} static s_GameTest1Subsystem;

KR_MAIN_GAME_MODULE(kr::DefaultMainModule);
