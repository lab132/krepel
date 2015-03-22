#pragma once

namespace krTests
{
  struct RaiiEngineStartup
  {
    RaiiEngineStartup() { ezStartup::StartupEngine(); }
    ~RaiiEngineStartup() { ezStartup::ShutdownEngine(); }
  };
}

/// \brief Scoped usage of the ezEngine 'Engine' core runtime.
#define KR_TESTS_RAII_ENGINE_STARTUP ::krTests::RaiiEngineStartup EZ_CONCAT(_engineStartup_, EZ_SOURCE_LINE)
