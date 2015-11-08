#include <krEngine/game.h>

class krGameTest1Module : public kr::DefaultMainModule
{
public:
  krGameTest1Module()
  {
    m_windowDesc.m_Title = "krGameTest1";

    EZ_VERIFY(ezFileSystem::AddDataDirectory(ezStringBuilder{ kr::defaultRoot(), "testData/textures" }, ezFileSystem::ReadOnly, "", "textures").Succeeded(),
              "Failed to mount textures directory.");
  }
} static g_mainModule; // <-- A static instance is mandatory!
