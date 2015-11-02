
#include <Foundation/Utilities/CommandLineUtils.h>
#include <Foundation/IO/ExtendedJSONReader.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Logging/VisualStudioWriter.h>

namespace
{
  struct JsonFileReader
  {
    ezFileReader file;
    ezJSONReader reader;
  };
}

static ezStringBuilder extractValue(const ezVariantDictionary& data, const char* key)
{
  ezVariant value;
  if(!data.TryGetValue(key, value))
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

static ezStringBuilder extractGameName(const ezCommandLineUtils& cmd, const JsonFileReader& json)
{
  const char* const key = "game";
  ezStringBuilder parentPath;
  ezStringBuilder gamePath = cmd.GetStringOption(key);

  if(gamePath.IsEmpty())
  {
    // Game path was not specified via command line switch.
    parentPath = json.file.GetFilePathAbsolute();
    parentPath.PathParentDirectory();
    gamePath = extractValue(json.reader.GetTopLevelObject(), key);
  }
  else
  {
    // Game path specified via command line.
    parentPath = kr::cwd();
  }

  parentPath.AppendPath(gamePath);
  return parentPath;
}

static ezResult loadGame(const ezStringBuilder& path)
{
  EZ_LOG_BLOCK("Loading Game", path);

  return ezPlugin::LoadPlugin(path);
}

int main(int argc, char* argv[])
{
  ezStringBuilder hello("Hello World");

  ezGlobalLog::AddLogWriter(ezLogWriter::Console::LogMessageHandler);
  KR_ON_SCOPE_EXIT{ ezGlobalLog::RemoveLogWriter(ezLogWriter::Console::LogMessageHandler); };

  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);
  KR_ON_SCOPE_EXIT{ ezGlobalLog::RemoveLogWriter(ezLogWriter::VisualStudio::LogMessageHandler); };

  ezStartup::StartupCore();
  KR_ON_SCOPE_EXIT{ ezStartup::ShutdownCore(); };

  ezFileSystem::RegisterDataDirectoryFactory(ezDataDirectory::FolderType::Factory);
  ezFileSystem::AddDataDirectory(kr::cwd().GetData(), ezFileSystem::DataDirUsage::AllowWrites, "cwd", "cwd");
  KR_ON_SCOPE_EXIT{ ezFileSystem::RemoveDataDirectoryGroup("cwd"); };

  ezCommandLineUtils cmd;
  cmd.SetCommandLine(argc, const_cast<const char**>(argv));

  ezStringBuilder configFileName = cmd.GetStringOption("--config");

  if(configFileName.IsEmpty())
  {
    configFileName = "config.json";
  }
  configFileName.Prepend("<cwd>");

  JsonFileReader json;
  EZ_VERIFY(json.file.Open(configFileName).Succeeded(), "Failed to open config file.");
  json.reader.Parse(json.file);

  auto gameName = extractGameName(cmd, json);
  if(gameName.IsEmpty())
  {
    ezLog::Error("No game path specified.");
    return 1;
  }

  // Finally load the game.
  loadGame(gameName);

  return 0;
}
