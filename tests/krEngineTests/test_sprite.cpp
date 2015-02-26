#include <krEngine/rendering/sprite.h>
#include <krEngine/rendering/renderer.h>
#include <krEngine/rendering/window.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Sprite);

EZ_CREATE_SIMPLE_TEST(Sprite, Workflow)
{
  using namespace kr;

  auto pWindow = Window::open();
  pWindow->setClearColor(ezColor::GetCornflowerBlue());

  Sprite s;
  s.setTexture(Texture::load("<texture>kitten.dds"));
  s.setBounds(ezRectU32(0, 0, 512, 512));
  initialize(s);

  ezStartup::StartupEngine();

  // Handle the close-event of the window.
  bool run = true;
  pWindow->getEvent().AddEventHandler([&run](const WindowEventArgs& e)
  {
    if (e.type == WindowEventArgs::ClickClose)
      run = false;
  });

  ezTime dt;
  while(run)
  {
    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(dt, pWindow, s);
  }

  ezStartup::ShutdownEngine();
}
