#pragma once

#include <Foundation/Types/Types.h>
#include <Foundation/Utilities/EnumerableClass.h>
#include <Foundation/Configuration/Plugin.h>

namespace kr
{
  class KR_ENGINE_API MainModule
  {
  public: // *** Static API
    static void installInstance(MainModule* pModule);
    static MainModule* instance();

  public: // *** Construction
    MainModule() {}
    virtual ~MainModule() {}

  public: // *** Events
    virtual void startupCore() = 0;
    virtual void startupEngine() = 0;
    virtual void shutdownEngine() = 0;
    virtual void shutdownCore() = 0;

    virtual void tick() = 0;

  public: // *** Accessors
    virtual bool keepTicking() const = 0;
  };

  template<typename ModuleType>
  void OnMainModuleLoad(bool isReloading)
  {
    static bool mainModuleInitialized = false;
    EZ_VERIFY(!mainModuleInitialized, "More than one main module specified! This is not allowed.");
    ModuleType* pModule = EZ_DEFAULT_NEW(ModuleType);
    kr::MainModule::installInstance(pModule);
  }

  template<typename ModuleType>
  void OnMainModuleUnload(bool isReloading)
  {
    auto pModule = static_cast<ModuleType*>(kr::MainModule::instance());
    kr::MainModule::installInstance(nullptr);
    EZ_DEFAULT_DELETE(pModule);
  }
}

#define KR_MAIN_GAME_MODULE(ModuleType) \
  static ezPlugin g_mainGameModulePlugin(false, &kr::OnMainModuleLoad<ModuleType>, &kr::OnMainModuleUnload<ModuleType>)
