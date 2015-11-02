#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/game.h>

TEST_CASE("Game Loop", "[game]")
{
  using namespace kr;

  SECTION("Ticking an empty main loop")
  {
    GameLoop loop;
    loop.tick();
  }

  SECTION("Single channel usage")
  {
    GameLoop loop;
    int check = 0;

    REQUIRE(loop.addCallback("foo", [&]() { ++check; }).Succeeded());
    REQUIRE(check == 0);

    loop.tick();
    REQUIRE(check == 1);

    loop.tick();
    loop.tick();
    REQUIRE(check == 3);

    REQUIRE(loop.removeCallback("foo").Succeeded());
    REQUIRE(check == 3);
    loop.tick();
    REQUIRE(check == 3);
  }

  SECTION("Multi-channel usage")
  {
    GameLoop loop;
    int checkFoo = 0;
    int checkBar = 0;

    auto cbBoth = [&]() { ++checkFoo; ++checkBar; };
    auto cbFoo = [&]() { ++checkFoo; };

    REQUIRE(loop.addCallback("fooAndBar", cbBoth).Succeeded());
    REQUIRE(loop.addCallback("foo", cbFoo).Succeeded());

    loop.tick();
    REQUIRE(checkFoo == 2);
    REQUIRE(checkBar == 1);

    loop.tick();
    loop.tick();
    REQUIRE(checkFoo == 6);
    REQUIRE(checkBar == 3);

    REQUIRE(loop.removeCallback("fooAndBar").Succeeded());
    loop.tick();
    REQUIRE(checkFoo == 7);
    REQUIRE(checkBar == 3);

    REQUIRE(loop.addCallback("fooAndBar", cbBoth).Succeeded());
    REQUIRE(loop.addCallback("fooAndBar", cbBoth).Failed());
    loop.tick();
    REQUIRE(checkFoo == 9);
    REQUIRE(checkBar == 4);
  }
}
