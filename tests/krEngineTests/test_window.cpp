#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/rendering/window.h>

TEST_CASE("Window creation", "[window]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  SECTION("create, no open or close")
  {
    auto window = Window::create();
    REQUIRE(window.isValid());

    KR_TESTS_RAII_ENGINE_STARTUP;
  }

  SECTION("create, open, explicit close")
  {
    auto window = Window::create();
    REQUIRE(window.isValid());
    REQUIRE(window->open().Succeeded());
    REQUIRE(window->close().Succeeded());

    KR_TESTS_RAII_ENGINE_STARTUP;
  }

  SECTION("create, open, implicit close")
  {
    auto window = Window::create();
    REQUIRE(window.isValid());
    REQUIRE(window->open().Succeeded());

    KR_TESTS_RAII_ENGINE_STARTUP;
  }

  SECTION("createAndOpen, implicit close")
  {
    auto window = Window::createAndOpen();
    REQUIRE(window.isValid());

    KR_TESTS_RAII_ENGINE_STARTUP;
  }
}
