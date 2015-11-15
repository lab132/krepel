#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/game.h>

TEST_CASE("Global Game Loop Registry", "[game]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  int check1 = 0;
  int check2 = 0;

  SECTION("Basics")
  {
    GlobalGameLoopRegistry::set("foo", [&]() { ++check1; });
    GlobalGameLoopRegistry::set("bar", [&]() { ++check2; });

    GlobalGameLoopRegistry::tick();
    REQUIRE(check1 == 1);
    REQUIRE(check2 == 1);

    REQUIRE(GlobalGameLoopRegistry::remove("foo").Succeeded());
    REQUIRE(GlobalGameLoopRegistry::remove("bar").Succeeded());
    REQUIRE(GlobalGameLoopRegistry::remove("bar").Failed());
  }

  SECTION("Priority")
  {
    GlobalGameLoopRegistry::set("bar", [&]() { REQUIRE(check1 == 1); ++check1; });
    GlobalGameLoopRegistry::set("foo", [&]() { REQUIRE(check1 == 0); ++check1; });

    GlobalGameLoopRegistry::setPriority("foo", 10);
    GlobalGameLoopRegistry::setPriority("bar", -10);

    GlobalGameLoopRegistry::tick();
    REQUIRE(check1 == 2);
    REQUIRE(check2 == 0);
  }
}
