#include <krEngineTests/pch.h>

#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/window.h>

EZ_CREATE_SIMPLE_TEST_GROUP(Texture);

EZ_CREATE_SIMPLE_TEST(Texture, Loading)
{
  using namespace kr;

  auto pWindow = Window::open();

  KR_TESTS_RAII_ENGINE_STARTUP;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Non-existant Texture File")
  {
    auto pTex = Texture::load("<GetOuttaHere!>I do not exist.nope");
    EZ_TEST_BOOL(isNull(pTex));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Load and Unload")
  {
    auto pTex = Texture::load("<texture>test_4x4.bmp");
    EZ_TEST_BOOL_MSG(isValid(pTex), "Unable to load image.");
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Multiple Load/Unload Calls")
  {
    auto pTex1 = Texture::load("<texture>test_4x4.bmp");
    EZ_TEST_BOOL(isValid(pTex1));
    auto pTex2 = Texture::load("<texture>test_4x4.bmp");
    EZ_TEST_BOOL(isValid(pTex1));

    EZ_TEST_BOOL_MSG(pTex1 == pTex2,
                      "Multiple Calls to Texture::load should result in the same handle!");
  }
}

EZ_CREATE_SIMPLE_TEST(Texture, DataAccess)
{
  using namespace kr;

  auto pWindow = Window::open();

  KR_TESTS_RAII_ENGINE_STARTUP;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Width and Height")
  {
    auto pTex = Texture::load("<texture>Test_4x4.bmp");

    EZ_TEST_INT(pTex->getWidth(), 4);
    EZ_TEST_INT(pTex->getHeight(), 4);
  }
}
