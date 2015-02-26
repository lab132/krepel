#include <krEngine/transform2D.h>
#include <krEngine/rendering.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Sprite);

EZ_CREATE_SIMPLE_TEST(Sprite, Workflow)
{
  using namespace kr;

  auto pWindow = Window::open();
  pWindow->setClearColor(ezColor::GetCornflowerBlue());

  Sprite s;
  s.setTexture(Texture::load("<texture>kitten.dds"));
  initialize(s);
  s.getSampler()->setFiltering(TextureFiltering::Nearest);

  ezStartup::StartupEngine();

  // Handle the close-event of the window.
  bool run = true;
  pWindow->getEvent().AddEventHandler([&run](const WindowEventArgs& e)
  {
    if (e.type == WindowEventArgs::ClickClose)
      run = false;
  });

  Renderer::addExtractionListener([&s](Renderer::Extractor& e)
  {
    Transform2D t(Zero);
    extract(e, s, t);
  });

  ezTime dt;
  while(run)
  {
    processWindowMessages(pWindow);
    Renderer::extract();
    Renderer::update(dt, pWindow);
  }

  ezStartup::ShutdownEngine();
}
