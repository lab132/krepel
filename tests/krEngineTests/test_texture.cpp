#include <krEngine/rendering/Texture.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Texture);

EZ_CREATE_SIMPLE_TEST(Texture, Loading)
{
  using namespace kr;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Non-existant Texture File")
  {
    auto hTex = Texture::load("<GetOuttaHere!>I do not exist.nope");
    EZ_TEST_BOOL(!hTex.isValid());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Load and Unload")
  {
    auto hTex = Texture::load("<Textures>Test_4x4.bmp");
    EZ_TEST_BOOL_MSG(hTex.isValid(), "Unable to load image.");

    Texture::unload(hTex);
    EZ_TEST_BOOL_MSG(!hTex.isValid(), "Handle should be invalid after the texture has been unloaded.");
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Multiple Load/Unload Calls")
  {
    auto hTex1 = Texture::load("<Textures>Test_4x4.bmp");
    EZ_TEST_BOOL(hTex1.isValid());
    auto hTex2 = Texture::load("<Textures>Test_4x4.bmp");
    EZ_TEST_BOOL(hTex2.isValid());

    EZ_TEST_BOOL_MSG(hTex1 == hTex2,
                     "Multiple Calls to Texture::load should result in the same handle!");

    Texture::unload(hTex1);
    EZ_TEST_BOOL(!hTex1.isValid());
    EZ_TEST_BOOL(hTex2.isValid());

    Texture::unload(hTex2);
    EZ_TEST_BOOL(!hTex1.isValid());
    EZ_TEST_BOOL(!hTex2.isValid());
  }
}

EZ_CREATE_SIMPLE_TEST(Texture, DataAccess)
{
  using namespace kr;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Width and Height")
  {
    auto hTex = Texture::load("<Textures>Test_4x4.bmp");

    EZ_TEST_INT(Texture::getWidth(hTex), 4);
    EZ_TEST_INT(Texture::getHeight(hTex), 4);

    Texture::unload(hTex);
  }
}
