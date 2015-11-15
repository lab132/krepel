#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/game.h>

TEST_CASE("Global Game Loop Registry", "[game]")
{
  using namespace kr;
  GlobalGameLoopRegistry::reset();
  KR_ON_SCOPE_EXIT{ GlobalGameLoopRegistry::reset(); };

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
}
