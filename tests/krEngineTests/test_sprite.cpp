#include <krEngine/rendering/sprite.h>
#include <krEngine/rendering/window.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Sprite);

EZ_CREATE_SIMPLE_TEST(Sprite, Basics)
{
  using namespace kr;

  auto pWindow = Window::open();
  pWindow->setClearColor(ezColor::GetCornflowerBlue());

  Sprite s;
  s.setTexture(Texture::load("<Textures>Test.png"));

  ezStartup::StartupEngine();

  ezStartup::ShutdownEngine();
}
