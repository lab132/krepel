#include <krEngine/game.h>

class krGameTest1Module : public kr::DefaultMainModule
{
public:
  krGameTest1Module()
  {
    m_windowDesc.m_Title = "krGameTest1";
  }
} static g_mainModule; // <-- A static instance is mandatory!
