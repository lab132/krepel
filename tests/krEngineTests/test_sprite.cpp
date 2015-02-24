#include <krEngine/rendering/Sprite.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Sprite);

EZ_CREATE_SIMPLE_TEST(Sprite, Basics)
{
  using namespace kr;

  Sprite s;
  auto hTex = Texture::load("<Textures>Test.png");
  //s.setTexture(hTex);

  // TODO Implement me.
}

#include <krEngine/rendering/Renderer.h>
#include <krEngine/rendering/Window.h>

EZ_CREATE_SIMPLE_TEST(Sprite, Display)
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

  EZ_TEST_BOOL(close(move(pWindow)).Succeeded());
}
