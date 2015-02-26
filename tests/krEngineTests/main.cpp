
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <TestFramework/Utilities/TestSetup.h>
#include <Foundation/Communication/GlobalEvent.h>

#include <Foundation/Logging/VisualStudioWriter.h>

static ezString64 g_texturesDir;
static ezString64 g_shadersDir;
static bool g_initialized = false;

EZ_ON_GLOBAL_EVENT(ezStartup_StartupCore_End)
{
  if (g_initialized)
    return;
  EZ_LOG_BLOCK("Common Test Setup", "Per Test");

  ezTestFramework::Output(ezTestOutput::Details,
                          "Mounting test data directories.");

  // Filesystem setup
  //////////////////////////////////////////////////////////////////////////
  // Register regular directory factory.
  ezFileSystem::RegisterDataDirectoryFactory(ezDataDirectory::FolderType::Factory);

  // Textures dir
  ezFileSystem::AddDataDirectory(g_texturesDir.GetData(),
                                 ezFileSystem::ReadOnly,
                                 "testData",
                                 "texture"); // To be used as"<texture>RedPants.dds"

  // Shaders dir
  ezFileSystem::AddDataDirectory(g_shadersDir.GetData(),
                                 ezFileSystem::ReadOnly,
                                 "testData",
                                 "shader"); // To be used as"<shader>Lighting.vs"

  ezGlobalLog::AddLogWriter(ezLogWriter::VisualStudio::LogMessageHandler);

  g_initialized = true;
}

EZ_ON_GLOBAL_EVENT(ezStartup_ShutdownCore_Begin)
{
  if (!g_initialized)
    return;

  EZ_LOG_BLOCK("Common Test Shutdown", "Per Test");

  ezTestFramework::Output(ezTestOutput::Details,
                          "Unmounting test data directories.");

  ezFileSystem::RemoveDataDirectoryGroup("TestData");

  g_initialized = false;
}

int main(int argc, char** argv)
{
  // Determine root dir.
  ezStringBuilder rootDir = ezOSFile::GetApplicationDirectory();
  rootDir.PathParentDirectory(3); // Three levels up.
  rootDir.MakeCleanPath();
  EZ_ASSERT(rootDir.IsAbsolutePath(), "Need absolute path.");

  // Textures dir
  {
    ezStringBuilder texDir = rootDir;
    texDir.AppendPath("testData", "textures");
    g_texturesDir = texDir;
  }

  // Shaders dir
  {
    ezStringBuilder shadersDir = rootDir;
    shadersDir.AppendPath("testData", "shaders");
    g_shadersDir = shadersDir;
  }

  // Set up and run tests.
  ezTestSetup::InitTestFramework("krEngineTests",
                                 "Krepel Engine Tests",
                                 argc,
                                 (const char**)argv);
  auto result = ezTestSetup::RunTests(argc, argv);
  ezTestSetup::DeInitTestFramework();
  return result;
}
