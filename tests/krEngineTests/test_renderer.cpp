#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/window.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Renderer);

EZ_CREATE_SIMPLE_TEST(Renderer, Experiments)
{
  using namespace kr;

  ezWindowCreationDesc desc;
  desc.m_ClientAreaSize.width = 600;
  desc.m_ClientAreaSize.height = 480;
  desc.m_Title = "Sprite Test";
  auto pWindow = Window::open(desc);
  EZ_TEST_BOOL(isValid(pWindow));

  ezStartup::StartupEngine();

  auto now = ezTime::Now();
  auto targetTime = now + ezTime::Seconds(2);
  while(now < targetTime)
  {
    auto dt = ezTime::Now() - now;

    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(dt, pWindow);

    now += dt;
  }

  ezStartup::ShutdownEngine();
}
