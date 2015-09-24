#include <krEngineTests/pch.h>
#include <catch.hpp>

TEST_CASE("ezArrayPtr Extension", "[common]")
{
  SECTION("Make From Plain Array")
  {
    int data[3] = { 3, 5, 7 };

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }

  SECTION("Make From ezStaticArray")
  {
    ezStaticArray<int, 3> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }

  SECTION("Make From ezHybridArray")
  {
    ezHybridArray<int, 2> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }

  SECTION("Make From ezDynamicArray")
  {
    ezDynamicArray<int> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }
}

TEST_CASE("ezContainer Extension", "[common]")
{
  SECTION("ezStaticArray")
  {
    ezStaticArray<int, 3> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    ezStaticArray<int, 3> result;
    for (auto i : data)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }

  SECTION("ezDynamicArray")
  {
    ezDynamicArray<int> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    ezStaticArray<int, 3> result;
    for (auto i : data)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }

  SECTION("ezHybridArray")
  {
    ezHybridArray<int, 2> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    ezStaticArray<int, 3> result;
    for (auto i : data)
    {
      result.PushBack(i);
    }

    REQUIRE(result[0] == 3);
    REQUIRE(result[1] == 5);
    REQUIRE(result[2] == 7);
  }

  /*
  SECTION("ezMap")
  {
    KR_NOT_IMPLEMENTED;
  }

  SECTION("ezHashTable")
  {
    KR_NOT_IMPLEMENTED;
  }
  */
}

TEST_CASE("Utils", "[common]")
{
  using namespace kr;

  int data = 42;
  auto pData = &data;

  REQUIRE(pData != nullptr);
  REQUIRE_FALSE(pData == nullptr);

  pData = nullptr;
  REQUIRE_FALSE(pData != nullptr);
  REQUIRE(pData == nullptr);
}
