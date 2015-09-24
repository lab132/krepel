#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/ownership.h>

struct Foo
{
  int data = 42;
};

template<typename T>
struct Bar
{
  kr::Borrowed<T> data;
};

int process(const Foo* foo)
{
  return foo->data * foo->data;
}

int process(const int* integer)
{
  int i = *integer;
  return 1 + i * i;
}

template<typename T>
int process(const Bar<T>& bar)
{
  return process(bar.data);
}

SCENARIO("OwnershipData Initialization", "[ownership]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  GIVEN("a default constructed object") {
    OwnershipData<char> data;

    WHEN("initialization finished") {
      THEN("the internal pointer must be null and the reference count zero.") {
        REQUIRE(data.ptr == 0);
        REQUIRE(data.refCount == 0);
      }
    }

    WHEN("adding a reference") {
      addRef(data);

      THEN("the reference count must be one.") {
        REQUIRE(data.refCount == 1);
      }
    }

    WHEN("removing a reference") {
      removeRef(data);

      THEN("the reference count must minus one.") {
        REQUIRE(data.refCount == -1);
      }
    }

    WHEN("adding and then removing a reference") {
      addRef(data);
      removeRef(data);

      THEN("the reference count must be minus zero.") {
        REQUIRE(data.refCount == 0);
      }
    }
  }
}

SCENARIO("taking ownership of some data.", "[ownership]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  GIVEN("some empty Owned structure")
  {
    Owned<int> o;

    REQUIRE(o.data.ptr == nullptr);
    REQUIRE(o.data.refCount == 0);

    WHEN("assigning it some data")
    {
      int deleteCount = 0;
      int data = 42;
      CAPTURE(&data);
      {
        o.assign(&data, [&deleteCount](int*){ ++deleteCount; });

        THEN("Dereferncing it yields the data")
        {
          REQUIRE(*o == 42);
        }

        THEN("the deleter was not invoked yet.")
        {
          REQUIRE(deleteCount == 0);
        }

        THEN("Owned points to the new data but the ref count is still zero.")
        {
          REQUIRE(o.data.ptr == &data);
          REQUIRE(o.data.refCount == 0);
        }
      }

      WHEN("Owned is reset")
      {
        o.reset();
        THEN("the deleter is invoked.")
        {
          REQUIRE(deleteCount == 1);
        }

        AND_THEN("Owned is no longer valid")
        {
          REQUIRE_FALSE(o != nullptr);
        }
      }

      WHEN("nullptr is assigned")
      {
        o = nullptr;
        THEN("the deleter is invoked.")
        {
          REQUIRE(deleteCount == 1);
        }

        AND_THEN("Owned is no longer valid")
        {
          REQUIRE_FALSE(o != nullptr);
        }
      }

      WHEN("borrowing this data")
      {
        Bar<int> bar{ borrow(o) };

        THEN("the ref count must be one.")
        {
          REQUIRE(o.data.refCount == 1);
        }

        THEN("the data can be used from the borrower.")
        {
          REQUIRE(process(bar) == (1 + 42 * 42));
        }
      }
    }
  }

  SECTION("Initialized with nullptr")
  {
    Owned<int> o{ nullptr };

    REQUIRE_FALSE(o != nullptr);
  }

  SECTION("Implicit conversion from nullptr to Owned")
  {
    auto func = []() -> Owned<int> { return nullptr; };
    REQUIRE_FALSE(func() != nullptr);
  }
}

TEST_CASE("kr::Owned", "[ownership]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  auto rawFooPtr = EZ_DEFAULT_NEW(Foo);
  auto foo = own(rawFooPtr, [](decltype(rawFooPtr) ptr){ EZ_DEFAULT_DELETE(ptr); });

  REQUIRE(foo.data.refCount == 0);
  REQUIRE(foo.data.ptr == rawFooPtr);

  REQUIRE(process(borrow(foo)) == 42 * 42);

  Bar<Foo> bar{ borrow(foo) };

  REQUIRE(process(bar.data) == 42 * 42);

  SECTION("Construction")
  {
    SECTION("Default")
    {
      Owned<char> o;
      REQUIRE(o.data.ptr == 0);
      REQUIRE(o.data.refCount == 0);
    }

    SECTION("Pointer And Deleter")
    {
      int count = 0;
      int data = 42;

      {
        Owned<int> o(&data, [&count](int*){ ++count; });

        // Proper initialization
        REQUIRE(bool(o));
        REQUIRE(o.data.ptr == &data);
        REQUIRE(o.data.refCount == 0);

        // Deleter not yet invoked.
        REQUIRE(count == 0);
      }

      REQUIRE(count == 1); // Deleter works.
    }
  }
}

void blackHole(kr::Owned<int> o)
{
  REQUIRE(bool(o));
  REQUIRE(o.data.refCount == 0);
}

TEST_CASE("Owned Move", "[ownership]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  int deleteCount = 0;
  int data = 42;
  Owned<int> o1(&data, [&deleteCount](int*){ ++deleteCount; });

  SECTION("Must not compile")
  {
    // Copy assignment.
    // auto o = o1;

    // Implicit copy.
    // blackHole(o1);
  }

  SECTION("Move ownership")
  {
    auto o2 = move(o1);

    REQUIRE_FALSE(bool(o1));
    REQUIRE(bool(o2));
    REQUIRE(deleteCount == 0);

    // Move ownership to blackHole.
    blackHole(move(o2));

    REQUIRE_FALSE(bool(o2));
    REQUIRE(deleteCount == 1);
  }

  SECTION("Yield Ownership")
  {
    auto dataPtr = o1.yieldOwnership();
    REQUIRE_FALSE(bool(o1));
    REQUIRE(o1.data.refCount == 0);
    REQUIRE(deleteCount == 0);
  }
}

TEST_CASE("Const Ownership", "[ownership]")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  int deleteCount = 0;
  int data = 1337;
  auto cleanUp = [&deleteCount](int*){ ++deleteCount; };
  const auto co = own(&data, cleanUp);

  SECTION("Must Not Compile")
  {
    //*co = 42;
    //*borrow(co) = 42;
  }

  SECTION("Data Access")
  {
    REQUIRE(*co == 1337);
  }

  SECTION("Borrow")
  {
    auto b = borrow(co);
    REQUIRE(b.pData->refCount == 1);
    REQUIRE(*b == 1337);
  }

  REQUIRE(co.data.refCount == 0);
}

TEST_CASE("Borrowed Conversion")
{
  using namespace kr;

  KR_TESTS_RAII_CORE_STARTUP;

  int data = 123;
  auto o = ownWithoutCleanUp(&data);

  SECTION("Implicitly convert Borrowed<T> to Borrowed<const T>")
  {
    auto func = [](kr::Borrowed<const int> b) -> int { return *b + 1; };

    auto b = borrow(o);
    auto result = func(b);
    REQUIRE(result == *b + 1);
  }
}
