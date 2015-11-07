#include <krEngine/game/mainModule.h>

static kr::MainModule* g_pMainModule{ nullptr };

void kr::MainModule::installInstance(MainModule* pModule)
{
  g_pMainModule = pModule;
}

kr::MainModule* kr::MainModule::instance()
{
  return g_pMainModule;
}
