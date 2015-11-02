#include <krEngine/game/gameLoop.h>

// Built-In Channels
kr::GameLoop* kr::GameLoop::preMainChannel()
{
  static GameLoop loop("preMain");
  return &loop;
}

kr::GameLoop* kr::GameLoop::mainChannel()
{
  static GameLoop loop("main");
  return &loop;
}

kr::GameLoop* kr::GameLoop::postMainChannel()
{
  static GameLoop loop("postMain");
  return &loop;
}

kr::GameLoop* kr::GameLoop::preRenderingChannel()
{
  static GameLoop loop("preRendering");
  return &loop;
}

kr::GameLoop* kr::GameLoop::renderingChannel()
{
  static GameLoop loop("rendering");
  return &loop;
}

kr::GameLoop* kr::GameLoop::postRenderingChannel()
{
  static GameLoop loop("postRendering");
  return &loop;
}

ezResult kr::GameLoop::addCallback(ezStringView callbackName, ezDelegate<void()> callback)
{
  EZ_LOG_BLOCK("kr::GameLoop::addCallback", m_name);

  auto pCallback{ getCallback(callbackName) };
  if(pCallback != nullptr)
  {
    ezLog::Warning("Callback named '%s' already exists.", ezStringBuilder(callbackName).GetData());
    return EZ_FAILURE;
  }

  pCallback = &m_callbacks.ExpandAndGetRef();
  pCallback->name = callbackName;
  pCallback->func = callback;

  return EZ_SUCCESS;
}

void kr::GameLoop::updateCallback(ezStringView callbackName, ezDelegate<void()> callback)
{
  EZ_LOG_BLOCK("kr::GameLoop::updateCallback", m_name);

  auto pCallback{ getCallback(callbackName) };
  if(pCallback == nullptr)
  {
    pCallback = &m_callbacks.ExpandAndGetRef();
  }

  pCallback->name = callbackName;
  pCallback->func = callback;
}

ezResult kr::GameLoop::removeCallback(ezStringView callbackName)
{
  EZ_LOG_BLOCK("kr::GameLoop::removeCallback", m_name);

  auto count{ m_callbacks.GetCount() };
  for(ezUInt32 i = 0; i < count; i++)
  {
    if(m_callbacks[i].name == callbackName)
    {
      m_callbacks.RemoveAt(i);
      return EZ_SUCCESS;
    }
  }

  ezLog::Error("No callback named '%s'.", ezStringBuilder(callbackName).GetData());
  return EZ_FAILURE;
}

void kr::GameLoop::tick(ezLogInterface* pLogInterface)
{
  EZ_LOG_BLOCK(pLogInterface, "Ticking GameLoop", m_name);
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
