#include <krEngineTests/pch.h>
#include <catch.hpp>

#include <krEngine/referenceCounting.h>

// Helpers
// =======
namespace
{
  class State : public kr::RefCounted
  {
  public: // *** Static API
    /// \brief We take care of releasing the state ourselves.
    using ReleasePolicy = State;

    /// \brief Will be called if a state object is no longer referenced.
    static void release(State*& pState)
    {
      REQUIRE(pState != nullptr);
      ++pState->numRelease;
      // Call the default release policy
      kr::RefCounted::ReleasePolicy::release(pState);
    }
  public:
    ezInt32 numAddRef = 0;
    ezInt32 numReleaseRef = 0;
    ezInt32 numRelease = 0;

  public:
    void addRef()
    {
      REQUIRE(this != nullptr);
      ++numAddRef;
      RefCounted::addRef();
    }

    void releaseRef()
    {
      REQUIRE(this != nullptr);
      ++numReleaseRef;
      RefCounted::releaseRef();
    }
  };

  struct StateOwner
  {
    kr::RefCountedPtr<State> pState;

    StateOwner(kr::RefCountedPtr<State> pState) :
      pState(move(pState))
    {
    }
  };
}

static bool operator ==(const State& lhs, const State& rhs)
{
  return lhs.numAddRef     == rhs.numAddRef
      && lhs.numReleaseRef == rhs.numReleaseRef
      && lhs.numRelease    == rhs.numRelease;
}

static bool operator !=(const State& lhs, const State& rhs)
{
  return !(lhs == rhs);
}

//////////////////////////////////////////////////////////////////////////


TEST_CASE("Free Functions", "[ref-counting]")
{
  using namespace kr;

  SECTION("xyzIfValid Functions")
  {
    State* p = nullptr;
    REQUIRE(addRefIfValid(p).Failed());
    REQUIRE(releaseRefIfValid(p).Failed());
  }

  SECTION("Standard Usage")
  {
    State s;
    auto S = &s;

    REQUIRE_FALSE(isReferenced(&s));

    addRef(S);
    REQUIRE(S != nullptr);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);
    REQUIRE(isReferenced(&s));

    addRef(S);
    REQUIRE(S != nullptr);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);
    REQUIRE(isReferenced(&s));

    releaseRef(S);
    REQUIRE(S == nullptr);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);
    REQUIRE(isReferenced(&s));

    S = &s;
    releaseRef(S);
    REQUIRE(S == nullptr);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);
    REQUIRE_FALSE(isReferenced(&s));
  }
}

template<typename T>
static void takeByConstRef(const kr::RefCountedPtr<T>&){}

template<typename T>
static void takeByValue(kr::RefCountedPtr<T>){}

template<typename T>
static void takeByRef(kr::RefCountedPtr<T>&){}

TEST_CASE("RefCountedPtr", "[ref-counting]")
{
  using namespace kr;

  SECTION("Construction")
  {
    State s;

    // Default construction.
    RefCountedPtr<State> p1;
    REQUIRE(p1.pRefCounted == nullptr);

    // Construct from pointer.
    RefCountedPtr<State> p2(&s);
    REQUIRE(p1.pRefCounted == nullptr);
    REQUIRE(p2.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Pointer assignment.
    p1 = &s;
    REQUIRE(p1.pRefCounted == &s);
    REQUIRE(p2.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Assigning the same pointer multiple times should not have any effect.
    for (int i = 0; i < 10; ++i)
    {
      p1 = &s;
    }
    REQUIRE(p1.pRefCounted == &s);
    REQUIRE(p2.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Copy construction.
    RefCountedPtr<State> p3(p2);
    REQUIRE(p1.pRefCounted == &s);
    REQUIRE(p2.pRefCounted == &s);
    REQUIRE(p3.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Copy assignment.
    p3 = p3; // Self assignment.
    p3 = nullptr; // Clear p3.
    p3 = p2;
    REQUIRE(p1.pRefCounted == &s);
    REQUIRE(p2.pRefCounted == &s);
    REQUIRE(p3.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Nullptr assignment.
    p3 = nullptr;
    REQUIRE(p1.pRefCounted == &s);
    REQUIRE(p2.pRefCounted == &s);
    REQUIRE(p3.pRefCounted == nullptr);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Move construction.
    RefCountedPtr<State> p4(move(p2));
    REQUIRE(p1.pRefCounted == &s);
    REQUIRE(p2.pRefCounted == nullptr);
    REQUIRE(p3.pRefCounted == nullptr);
    REQUIRE(p4.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Move assignment.
    p4 = move(p4); // Self assignment.
    p4 = move(p1);
    REQUIRE(p1.pRefCounted == nullptr);
    REQUIRE(p2.pRefCounted == nullptr);
    REQUIRE(p3.pRefCounted == nullptr);
    REQUIRE(p4.pRefCounted == &s);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);

    // Test scope behavior
    {
      RefCountedPtr<State> killEmAll(move(p4));
      REQUIRE(p1.pRefCounted == nullptr);
      REQUIRE(p2.pRefCounted == nullptr);
      REQUIRE(p3.pRefCounted == nullptr);
      REQUIRE(p4.pRefCounted == nullptr);
      REQUIRE(killEmAll.pRefCounted == &s);
      REQUIRE(s.numAddRef == s.numAddRef);
      REQUIRE(s.numReleaseRef == s.numReleaseRef);
      REQUIRE(s.numRelease == s.numRelease);
    }

    REQUIRE(p1.pRefCounted == nullptr);
    REQUIRE(p2.pRefCounted == nullptr);
    REQUIRE(p3.pRefCounted == nullptr);
    REQUIRE(p4.pRefCounted == nullptr);
    REQUIRE(s.numAddRef == s.numAddRef);
    REQUIRE(s.numReleaseRef == s.numReleaseRef);
    REQUIRE(s.numRelease == s.numRelease);
    REQUIRE_FALSE(isReferenced(&s));
  }

  SECTION("Correctness of makeRefCountedPtr")
  {
    State s;

    {
      auto p = makeRefCountedPtr(&s);

      REQUIRE(s.numAddRef == 1);
      REQUIRE(s.numReleaseRef == 0);
      REQUIRE(s.numRelease == 0);
    }

    REQUIRE(s.numAddRef == 1);
    REQUIRE(s.numReleaseRef == 1);
    REQUIRE(s.numRelease == 1);
  }

  SECTION("Ownership")
  {
    State s;

    auto p = makeRefCountedPtr(&s);
    StateOwner o(move(p));

    REQUIRE(p.pRefCounted == nullptr);
    REQUIRE(o.pState.pRefCounted == &s);
    REQUIRE(s.numAddRef == 1);
    REQUIRE(s.numReleaseRef == 0);
    REQUIRE(s.numRelease == 0);
  }

  SECTION("Passing as Argument")
  {
    State s;
    auto p = makeRefCountedPtr(&s);

    takeByConstRef(p);
    REQUIRE(p.pRefCounted == &s);
    REQUIRE(s.numAddRef == 1);
    REQUIRE(s.numReleaseRef == 0);
    REQUIRE(s.numRelease == 0);

    // Since `move` is just a cast, moving into a const-ref receiver
    // should not do anything.
    takeByConstRef(move(p));
    REQUIRE(p.pRefCounted == &s);
    REQUIRE(s.numAddRef == 1);
    REQUIRE(s.numReleaseRef == 0);
    REQUIRE(s.numRelease == 0);

    takeByRef(p);
    REQUIRE(p.pRefCounted == &s);
    REQUIRE(s.numAddRef == 1);
    REQUIRE(s.numReleaseRef == 0);
    REQUIRE(s.numRelease == 0);

    // Same as with const-ref
    takeByRef(move(p));
    REQUIRE(p.pRefCounted == &s);
    REQUIRE(s.numAddRef == 1);
    REQUIRE(s.numReleaseRef == 0);
    REQUIRE(s.numRelease == 0);

    // Should make a copy and release it again.
    takeByValue(p);
    REQUIRE(p.pRefCounted == &s);
    REQUIRE(s.numAddRef == 2);
    REQUIRE(s.numReleaseRef == 1);
    REQUIRE(s.numRelease == 0);

    // Should move 'ownership' to the function, which releases its reference,
    // releasing all existing references in our case.
    takeByValue(move(p));
    REQUIRE(p.pRefCounted == nullptr);
    REQUIRE(s.numAddRef == 2);
    REQUIRE(s.numReleaseRef == 2);
    REQUIRE(s.numRelease == 1);
  }
}
