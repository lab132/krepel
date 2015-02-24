#include <krEngine/rendering/sprite.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Sprite);

EZ_CREATE_SIMPLE_TEST(Sprite, Basics)
{
  using namespace kr;

  Sprite s;
  auto hTex = Texture::load("<Textures>Test.png");
  //s.setTexture(hTex);

  // TODO Implement me.
}
