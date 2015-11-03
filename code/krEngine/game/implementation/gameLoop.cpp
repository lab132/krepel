#include <krEngine/game/gameLoop.h>

ezResult kr::GameLoop::addCallback(ezStringView callbackName, ezDelegate<void()> callback)
{
  EZ_LOG_BLOCK("kr::GameLoop::addCallback", ezStringBuilder(callbackName).GetData());

  auto pCallback{ getCallback(callbackName) };
  if(pCallback != nullptr)
  {
    ezLog::Warning("Callback with the given name already exists.");
    return EZ_FAILURE;
  }

  ezLog::VerboseDebugMessage("Adding callback.");

  pCallback = &m_callbacks.ExpandAndGetRef();
  pCallback->name = callbackName;
  pCallback->func = callback;

  return EZ_SUCCESS;
}

void kr::GameLoop::updateCallback(ezStringView callbackName, ezDelegate<void()> callback)
{
  EZ_LOG_BLOCK("kr::GameLoop::updateCallback", ezStringBuilder(callbackName).GetData());

  auto pCallback{ getCallback(callbackName) };
  if(pCallback)
  {
    ezLog::VerboseDebugMessage("Overwriting existing instance.");
  }
  else
  {
    ezLog::VerboseDebugMessage("Creating new instance.");
    pCallback = &m_callbacks.ExpandAndGetRef();
  }

  pCallback->name = callbackName;
  pCallback->func = callback;
}

ezResult kr::GameLoop::removeCallback(ezStringView callbackName)
{
  EZ_LOG_BLOCK("kr::GameLoop::removeCallback", ezStringBuilder(callbackName).GetData());

  auto count{ m_callbacks.GetCount() };
  for(ezUInt32 i = 0; i < count; i++)
  {
    if(m_callbacks[i].name == callbackName)
    {
      m_callbacks.RemoveAt(i);
      return EZ_SUCCESS;
    }
  }

  ezLog::Info("Unable to find the callback with the given name.");
  return EZ_FAILURE;
}

void kr::GameLoop::tick(ezLogInterface* pLogInterface)
{
  for(auto& callback : m_callbacks)
  {
    ezStringBuilder callbackName{ callback.name };
    EZ_LOG_BLOCK(pLogInterface, "Ticking Callback", callbackName);

    callback.func();
  }
}

kr::GameLoop::Callback* kr::GameLoop::getCallback(ezStringView name)
{
  for(auto& cb : m_callbacks)
  {
    if(cb.name == name)
    {
      return &cb;
    }
  }

  return nullptr;
}

namespace
{
  struct NamedGameLoop
  {
    ezString name;
    kr::GameLoop* pInstance = nullptr;

    NamedGameLoop() = default;
    NamedGameLoop(ezStringView name, kr::GameLoop* pInstance) : name{ name }, pInstance{ pInstance } {}
    ~NamedGameLoop() { pInstance = nullptr; }
  };
}

static ezDynamicArray<NamedGameLoop, ezStaticAllocatorWrapper>& globalGameLoops()
{
  static ezDynamicArray<NamedGameLoop, ezStaticAllocatorWrapper> value;
  return value;
}

ezResult kr::GlobalGameLoopRegistry::add(ezStringView loopName, GameLoop* pLoop, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Add Global Game Loop", ezStringBuilder(loopName).GetData());

  if(pLoop == nullptr)
  {
    ezLog::Warning(pLogInterface, "Given game loop instance is null.");
    return EZ_FAILURE;
  }

  auto& all = globalGameLoops();
  for(auto& namedLoop : all)
  {
    if(namedLoop.pInstance == pLoop)
    {
      ezLog::Info(pLogInterface, "Global game loop with the given name already exists.");
      ezLog::Dev(pLogInterface, "If you want to overwrite it, call remove() first.");
      return EZ_FAILURE;
    }
  }

  all.PushBack(move(NamedGameLoop{ loopName, pLoop }));
  ezLog::Success(pLogInterface, "Game loop added successfully.");
  return EZ_SUCCESS;
}

kr::GameLoop* kr::GlobalGameLoopRegistry::get(ezStringView loopName, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Get Global Game Loop");

  auto& all = globalGameLoops();
  for(auto& namedLoop : all)
  {
    if(namedLoop.name == loopName)
    {
      ezLog::VerboseDebugMessage(pLogInterface, "Found global game loop named '%s'.", loopName);
      return namedLoop.pInstance;
    }
  }

  ezLog::Warning(pLogInterface, "Unable to find global game loop named '%s'.", ezStringBuilder(loopName).GetData());
  return nullptr;
}

ezResult kr::GlobalGameLoopRegistry::remove(GameLoop* pLoop, ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Removing Global Game Loop");

  if(pLoop == nullptr)
  {
    ezLog::Warning(pLogInterface, "Given game loop instance is null!");
    return EZ_FAILURE;
  }

  auto& all = globalGameLoops();
  for(ezUInt32 i = 0; i < all.GetCount(); ++i)
  {
    if(all[i].pInstance == pLoop)
    {
      ezLog::Success(pLogInterface, "Removing game loop named '%s'.", all[i].name);
      all.RemoveAt(i);
      return EZ_SUCCESS;
    }
  }

  ezLog::Warning(pLogInterface, "Given game loop instance is not registered. Unable to remove.");
  return EZ_FAILURE;
}

void kr::GlobalGameLoopRegistry::tick(ezLogInterface* pLogInterface)
{
  for(auto& namedLoop : globalGameLoops())
  {
    EZ_LOG_BLOCK(pLogInterface, "Ticking Global Game Loop", namedLoop.name);
    namedLoop.pInstance->tick(pLogInterface);
  }
}
