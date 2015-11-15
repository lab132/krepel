#include <krEngine/game/gameLoop.h>

#include <Foundation/Configuration/SubSystem.h>


EZ_BEGIN_SUBSYSTEM_DECLARATION(krEngine, GlobalGameLoopRegistry)
ON_CORE_SHUTDOWN
{
  kr::GlobalGameLoopRegistry::reset();
}
EZ_END_SUBSYSTEM_DECLARATION


namespace
{
  struct GameLoop
  {
    bool isGarbage{ false };
    ezString name;
    kr::GameLoopCallback callback;

    GameLoop(ezStringView name, kr::GameLoopCallback cb) : name{ kr::move(name) }, callback{ kr::move(cb) }
    {
    }
  };
}

static ezDynamicArray<GameLoop, ezStaticAllocatorWrapper>& globalGameLoops()
{
  static ezDynamicArray<GameLoop, ezStaticAllocatorWrapper> value;
  return value;
}

void kr::GlobalGameLoopRegistry::set(ezStringView loopName, GameLoopCallback callback, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Add Global Game Loop", ezStringBuilder(loopName).GetData());

  if (!callback.IsValid())
  {
    ezLog::Warning(pLogInterface, "The given game loop callback is not valid.");
  }

  auto& all = globalGameLoops();
  for(auto& loop : all)
  {
    if(loop.name == loopName)
    {
      ezLog::Info(pLogInterface, "Overwriting existing game loop.");
      loop.callback = move(callback);
      return;
    }
  }

  all.PushBack(move(GameLoop{ move(ezString{ loopName }), move(callback) }));
  ezLog::Success(pLogInterface, "Game loop added successfully.");
}

kr::GameLoopCallback* kr::GlobalGameLoopRegistry::get(ezStringView loopName, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Get Global Game Loop", ezStringBuilder{ loopName });

  auto& all = globalGameLoops();
  for(auto& loop : all)
  {
    if(loop.name == loopName)
    {
      ezLog::VerboseDebugMessage(pLogInterface, "Found global game loop with the given name.");
      return &loop.callback;
    }
  }

  ezLog::Warning(pLogInterface, "Unable to find global game loop with the given name.");
  return nullptr;
}

ezResult kr::GlobalGameLoopRegistry::remove(ezStringView loopName, ezLogInterface* pLogInterface /*= nullptr*/)
{
  EZ_LOG_BLOCK(pLogInterface, "Removing Global Game Loop", ezStringBuilder(loopName));

  auto& all{ globalGameLoops() };
  for(ezUInt32 i = 0; i < all.GetCount(); ++i)
  {
    if(all[i].name == loopName)
    {
      if (GlobalGameLoopRegistry::isTicking())
      {
        // While ticking, we cannot simply remove the loop. We mark it as garbage and remove them once we're done ticking.
        all[i].isGarbage = true;
      }
      else
      {
        // Since we are not ticking the global game loop, we can immediately remove it.
        all.RemoveAt(i);
      }

      ezLog::Success(pLogInterface, "Removed game loop named '%s'.");
      return EZ_SUCCESS;
    }
  }

  ezLog::Warning(pLogInterface, "Given game loop instance is not registered. Unable to remove.");
  return EZ_FAILURE;
}

static bool g_globalGameLoopIsTicking{ false };

void kr::GlobalGameLoopRegistry::tick(ezLogInterface* pLogInterface)
{
  g_globalGameLoopIsTicking = true;
  KR_ON_SCOPE_EXIT{ g_globalGameLoopIsTicking = false; };

  // Tick all loops.
  auto& all = globalGameLoops();
  for(auto& loop : all)
  {
    EZ_LOG_BLOCK(pLogInterface, "Ticking Global Game Loop", loop.name);
    if (!loop.isGarbage && loop.callback.IsValid())
    {
      loop.callback();
    }
  }

  // Collect Garbage
  // ===============
  ezUInt32 leftToCheck{ all.GetCount() };
  while(leftToCheck > 0)
  {
    auto index = leftToCheck - 1;
    if (all[index].isGarbage)
    {
      all.RemoveAt(index);
    }
    --leftToCheck;
  }
}

static bool g_keepGlobalGameLoopTicking{ true };

bool kr::GlobalGameLoopRegistry::keepTicking()
{
  return g_keepGlobalGameLoopTicking;
}

void kr::GlobalGameLoopRegistry::setKeepTicking(bool value)
{
  g_keepGlobalGameLoopTicking = value;
}

bool kr::GlobalGameLoopRegistry::isTicking()
{
  return g_globalGameLoopIsTicking;
}

void kr::GlobalGameLoopRegistry::printTickOrder(ezLogInterface* pLogInterface /*= nullptr*/)
{
  for (auto& loop : globalGameLoops())
  {
    ezLog::Info(pLogInterface, "%s", loop.name);
  }
}

void kr::GlobalGameLoopRegistry::reset()
{
  globalGameLoops().Clear();
}
