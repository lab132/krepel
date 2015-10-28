#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/rendering/texture.h>
#include <krEngine/rendering/window.h>

TEST_CASE("Loading", "[texture]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  auto pWindow = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  SECTION("Non-existant Texture File")
  {
    auto pTex = Texture::load("<GetOuttaHere!>I do not exist.nope");
    REQUIRE(pTex == nullptr);
  }

  SECTION("Load and Unload")
  {
    auto pTex = Texture::load("<texture>test_4x4.bmp");
    REQUIRE(pTex != nullptr);
  }

  SECTION("Multiple Load/Unload Calls")
  {
    auto pTex1 = Texture::load("<texture>test_4x4.bmp");
    REQUIRE(pTex1 != nullptr);
    auto pTex2 = Texture::load("<texture>test_4x4.bmp");
    REQUIRE(pTex2 != nullptr);

    // Multiple Calls to Texture::load should not result in the same handle!
    // This is be the job of a resource management system.
    REQUIRE(pTex1 != pTex2);
  }
}

TEST_CASE("Data Access", "[texture]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  auto pWindow = Window::createAndOpen();

  KR_TESTS_RAII_ENGINE_STARTUP;

  SECTION("Width and Height")
  {
    auto pTex = Texture::load("<texture>Test_4x4.bmp");

    REQUIRE(pTex->getWidth() == 4u);
    REQUIRE(pTex->getHeight() == 4u);
  }
}
