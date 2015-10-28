#pragma once

namespace krTests
{
  struct RaiiCoreStartup
  {
    RaiiCoreStartup() { ezStartup::StartupCore(); }
    ~RaiiCoreStartup() { ezStartup::ShutdownCore(); }
  };

  struct RaiiEngineStartup
  {
    RaiiEngineStartup() { ezStartup::StartupEngine(); }
    ~RaiiEngineStartup() { ezStartup::ShutdownEngine(); }
  };
}

/// \brief Scoped usage of the ezEngine 'Core' runtime.
#define KR_TESTS_RAII_CORE_STARTUP ::krTests::RaiiCoreStartup EZ_CONCAT(_coreStartup_, EZ_SOURCE_LINE)

/// \brief Scoped usage of the ezEngine 'Engine' core runtime.
#define KR_TESTS_RAII_ENGINE_STARTUP ::krTests::RaiiEngineStartup EZ_CONCAT(_engineStartup_, EZ_SOURCE_LINE)
