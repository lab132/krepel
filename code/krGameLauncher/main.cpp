#include <krEngine/game/mainModule.h>

#include <Foundation/Utilities/CommandLineUtils.h>
#include <Foundation/IO/ExtendedJSONReader.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>

namespace
{
  struct JsonFileReader
  {
    ezOSFile file;
    ezJSONReader reader;
  };
}

// pData may NOT be a nullptr.
static ezStringBuilder extractValue(const ezVariantDictionary* pData, const char* key)
{
  EZ_ASSERT_DEBUG(pData != nullptr, "Unexpected nullptr.");

  ezVariant value;
  if(!pData->TryGetValue(key, value))
  {
    ezLog::Error("Unable to find key '%s'.", key);
    return ezStringBuilder();
  }

  if(value.GetType() != ezVariantType::String)
  {
    ezLog::Error("Expected a string value for the key '%s'.", key);
    return ezStringBuilder();
  }

  return ezStringBuilder(value.Get<ezString>());
}

// pData may be a nullptr.
static ezStringBuilder extractValue(const ezCommandLineUtils& cmd, const ezVariantDictionary* pData, const char* key)
{
  ezStringBuilder gamePath = cmd.GetStringOption(key);

  if(gamePath.IsEmpty() && pData != nullptr)
  {
    // Game path was not specified via command line switch.
    gamePath = extractValue(pData, key);
  }

  return gamePath;
}

static ezResult loadGame(const ezStringBuilder& name)
{
  EZ_LOG_BLOCK("Loading Game", name);

  return ezPlugin::LoadPlugin(name);
}

static ezResult unloadGame(const ezStringBuilder& name)
{
  EZ_LOG_BLOCK("Unloading Game", name);

  return ezPlugin::UnloadPlugin(name);
}

static int runGame(const ezStringBuilder& name)
{
  EZ_LOG_BLOCK("Running Game", name);

  auto pModule{ kr::MainModule::instance() };

  if (pModule == nullptr)
  {
    ezLog::Error("No main module instance found.");
    return 1;
  }

  pModule->startupCore();
  pModule->startupEngine();

  while(pModule->keepTicking())
  {
    pModule->tick();
  }

  pModule->shutdownEngine();
  pModule->shutdownCore();

  return 0;
}

int main(int argc, char* argv[])
{
  ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
  KR_ON_SCOPE_EXIT{ ezGlobalLog::RemoveLogWriter(ezLogWriter::Console::LogMessageHandler); };
  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);
  KR_ON_SCOPE_EXIT{ ezGlobalLog::RemoveLogWriter(ezLogWriter::VisualStudio::LogMessageHandler); };

  ezCommandLineUtils cmd;
  cmd.SetCommandLine(argc, const_cast<const char**>(argv));

  ezStringBuilder configFileName{ cmd.GetStringOption("--config") };

  if(configFileName.IsEmpty())
  {
    configFileName = "config.json";
  }

  const ezVariantDictionary* pData{ nullptr };
  ezExtendedJSONReader jsonReader;
  ezOSFile jsonFile;

  if (jsonFile.Open(configFileName, ezFileMode::Read).Failed())
  {
    ezLog::Info("Unable to open JSON config file.");
  }
  else
  {
    auto fileData{ EZ_DEFAULT_NEW_ARRAY(ezUInt8, static_cast<ezUInt32>(jsonFile.GetFileSize())) };
    jsonFile.Read(fileData.GetPtr(), fileData.GetCount());

    {
      ezMemoryStreamStorage mem{ fileData.GetCount() };
      ezMemoryStreamWriter{ &mem }.WriteBytes(fileData.GetPtr(), fileData.GetCount());
      if(jsonReader.Parse(ezMemoryStreamReader{ &mem }).Failed())
      {
        ezLog::SeriousWarning("Discarding config file due to error when parsing. Check for a valid JSON format.");
      }
      else
      {
        pData = &jsonReader.GetTopLevelObject();
      }
    }
  }

  auto gameName{ extractValue(cmd, pData, "--game") };
  if(gameName.IsEmpty())
  {
    ezLog::Error("No game module specified. use either --game <name> on the command line or \"game\" = \"<name>\" in the config file.");
    return -1;
  }

  if(loadGame(gameName).Failed())
  {
    ezLog::Error("Failed to load game.");
    return 1;
  }

  auto result{ runGame(gameName) };
  if(result != 0)
  {
    return result;
  }

  if(unloadGame(gameName).Failed())
  {
    ezLog::Error("Failed to unload game.");
    return 1;
  }

  return 0;
}
