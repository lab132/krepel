#include <krEngine/ReferenceCounting.h>

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
      EZ_TEST_BOOL(pState != nullptr);
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
      EZ_TEST_BOOL(this != nullptr);
      ++numAddRef;
      RefCounted::addRef();
    }

    void releaseRef()
    {
      EZ_TEST_BOOL(this != nullptr);
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

EZ_CREATE_SIMPLE_TEST_GROUP(ReferenceCounting);

EZ_CREATE_SIMPLE_TEST(ReferenceCounting, FreeFunctions)
{
  using namespace kr;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "xyzIfValid Functions")
  {
    State* p = nullptr;
    EZ_TEST_BOOL(addRefIfValid(p).Failed());
    EZ_TEST_BOOL(releaseRefIfValid(p).Failed());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Standard Usage")
  {
    State s;
    auto S = &s;

    EZ_TEST_BOOL(!isReferenced(&s));

    addRef(S);
    EZ_TEST_BOOL(S != nullptr);
    EZ_TEST_INT(s.numAddRef, 1);
    EZ_TEST_INT(s.numReleaseRef, 0);
    EZ_TEST_INT(s.numRelease, 0);
    EZ_TEST_BOOL(isReferenced(&s));

    addRef(S);
    EZ_TEST_BOOL(S != nullptr);
    EZ_TEST_INT(s.numAddRef, 2);
    EZ_TEST_INT(s.numReleaseRef, 0);
    EZ_TEST_INT(s.numRelease, 0);
    EZ_TEST_BOOL(isReferenced(&s));

    releaseRef(S);
    EZ_TEST_BOOL(S == nullptr);
    EZ_TEST_INT(s.numAddRef, 2);
    EZ_TEST_INT(s.numReleaseRef, 1);
    EZ_TEST_INT(s.numRelease, 0);
    EZ_TEST_BOOL(isReferenced(&s));

    S = &s;
    releaseRef(S);
    EZ_TEST_BOOL(S == nullptr);
    EZ_TEST_INT(s.numAddRef, 2);
    EZ_TEST_INT(s.numReleaseRef, 2);
    EZ_TEST_INT(s.numRelease, 1);
    EZ_TEST_BOOL(!isReferenced(&s));
  }
}

template<typename T>
static void takeByConstRef(const kr::RefCountedPtr<T>&){}

template<typename T>
static void takeByValue(kr::RefCountedPtr<T>){}

template<typename T>
static void takeByRef(kr::RefCountedPtr<T>&){}

EZ_CREATE_SIMPLE_TEST(ReferenceCounting, RefCountedPtr)
{
  using namespace kr;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Construction")
  {
    State s;

    // Default construction.
    RefCountedPtr<State> p1;
    EZ_TEST_BOOL(p1.pRefCounted == nullptr);

    // Construct from pointer.
    RefCountedPtr<State> p2(&s);
    EZ_TEST_BOOL(p1.pRefCounted == nullptr);
    EZ_TEST_BOOL(p2.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 1);
    EZ_TEST_INT(s.numReleaseRef, 0);
    EZ_TEST_INT(s.numRelease, 0);

    // Pointer assignment.
    p1 = &s;
    EZ_TEST_BOOL(p1.pRefCounted == &s);
    EZ_TEST_BOOL(p2.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 2);
    EZ_TEST_INT(s.numReleaseRef, 0);
    EZ_TEST_INT(s.numRelease, 0);

    // Assigning the same pointer multiple times should not have any effect.
    for (int i = 0; i < 10; ++i)
    {
      p1 = &s;
    }
    EZ_TEST_BOOL(p1.pRefCounted == &s);
    EZ_TEST_BOOL(p2.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 2);
    EZ_TEST_INT(s.numReleaseRef, 0);
    EZ_TEST_INT(s.numRelease, 0);

    // Copy construction.
    RefCountedPtr<State> p3(p2);
    EZ_TEST_BOOL(p1.pRefCounted == &s);
    EZ_TEST_BOOL(p2.pRefCounted == &s);
    EZ_TEST_BOOL(p3.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 3);
    EZ_TEST_INT(s.numReleaseRef, 0);
    EZ_TEST_INT(s.numRelease, 0);

    // Copy assignment.
    p3 = p3; // Self assignment.
    p3 = nullptr; // Clear p3.
    p3 = p2;
    EZ_TEST_BOOL(p1.pRefCounted == &s);
    EZ_TEST_BOOL(p2.pRefCounted == &s);
    EZ_TEST_BOOL(p3.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 4);
    EZ_TEST_INT(s.numReleaseRef, 1);
    EZ_TEST_INT(s.numRelease, 0);

    // Nullptr assignment.
    p3 = nullptr;
    EZ_TEST_BOOL(p1.pRefCounted == &s);
    EZ_TEST_BOOL(p2.pRefCounted == &s);
    EZ_TEST_BOOL(p3.pRefCounted == nullptr);
    EZ_TEST_INT(s.numAddRef, 4);
    EZ_TEST_INT(s.numReleaseRef, 2);
    EZ_TEST_INT(s.numRelease, 0);

    // Move construction.
    RefCountedPtr<State> p4(move(p2));
    EZ_TEST_BOOL(p1.pRefCounted == &s);
    EZ_TEST_BOOL(p2.pRefCounted == nullptr);
    EZ_TEST_BOOL(p3.pRefCounted == nullptr);
    EZ_TEST_BOOL(p4.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 4);
    EZ_TEST_INT(s.numReleaseRef, 2);
    EZ_TEST_INT(s.numRelease, 0);

    // Move assignment.
    p4 = move(p4); // Self assignment.
    p4 = move(p1);
    EZ_TEST_BOOL(p1.pRefCounted == nullptr);
    EZ_TEST_BOOL(p2.pRefCounted == nullptr);
    EZ_TEST_BOOL(p3.pRefCounted == nullptr);
    EZ_TEST_BOOL(p4.pRefCounted == &s);
    EZ_TEST_INT(s.numAddRef, 4);
    EZ_TEST_INT(s.numReleaseRef, 3);
    EZ_TEST_INT(s.numRelease, 0);

    // Test scope behavior
    {
      RefCountedPtr<State> killEmAll(move(p4));
      EZ_TEST_BOOL(p1.pRefCounted == nullptr);
      EZ_TEST_BOOL(p2.pRefCounted == nullptr);
      EZ_TEST_BOOL(p3.pRefCounted == nullptr);
      EZ_TEST_BOOL(p4.pRefCounted == nullptr);
      EZ_TEST_BOOL(killEmAll.pRefCounted == &s);
      EZ_TEST_INT(s.numAddRef, 4);
      EZ_TEST_INT(s.numReleaseRef, 3);
      EZ_TEST_INT(s.numRelease, 0);
    }

    EZ_TEST_BOOL(p1.pRefCounted == nullptr);
    EZ_TEST_BOOL(p2.pRefCounted == nullptr);
    EZ_TEST_BOOL(p3.pRefCounted == nullptr);
    EZ_TEST_BOOL(p4.pRefCounted == nullptr);
    EZ_TEST_INT(s.numAddRef, 4);
    EZ_TEST_INT(s.numReleaseRef, 4);
    EZ_TEST_INT(s.numRelease, 1);
    EZ_TEST_BOOL(!isReferenced(&s));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Correctness of makeRefCountedPtr")
  {
    State s;

    {
      auto p = makeRefCountedPtr(&s);

      EZ_TEST_BOOL(s.numAddRef == 1);
      EZ_TEST_BOOL(s.numReleaseRef == 0);
      EZ_TEST_BOOL(s.numRelease == 0);
    }

    EZ_TEST_BOOL(s.numAddRef == 1);
    EZ_TEST_BOOL(s.numReleaseRef == 1);
    EZ_TEST_BOOL(s.numRelease == 1);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Ownership")
  {
    State s;

    auto p = makeRefCountedPtr(&s);
    StateOwner o(move(p));

    EZ_TEST_BOOL(p.pRefCounted == nullptr);
    EZ_TEST_BOOL(o.pState.pRefCounted == &s);
    EZ_TEST_BOOL(s.numAddRef == 1);
    EZ_TEST_BOOL(s.numReleaseRef == 0);
    EZ_TEST_BOOL(s.numRelease == 0);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Passing as Argument")
  {
    State s;
    auto p = makeRefCountedPtr(&s);

    takeByConstRef(p);
    EZ_TEST_BOOL(p.pRefCounted == &s);
    EZ_TEST_BOOL(s.numAddRef == 1);
    EZ_TEST_BOOL(s.numReleaseRef == 0);
    EZ_TEST_BOOL(s.numRelease == 0);

    // Since `move` is just a cast, moving into a const-ref receiver
    // should not do anything.
    takeByConstRef(move(p));
    EZ_TEST_BOOL(p.pRefCounted == &s);
    EZ_TEST_BOOL(s.numAddRef == 1);
    EZ_TEST_BOOL(s.numReleaseRef == 0);
    EZ_TEST_BOOL(s.numRelease == 0);

    takeByRef(p);
    EZ_TEST_BOOL(p.pRefCounted == &s);
    EZ_TEST_BOOL(s.numAddRef == 1);
    EZ_TEST_BOOL(s.numReleaseRef == 0);
    EZ_TEST_BOOL(s.numRelease == 0);

    // Same as with const-ref
    takeByRef(move(p));
    EZ_TEST_BOOL(p.pRefCounted == &s);
    EZ_TEST_BOOL(s.numAddRef == 1);
    EZ_TEST_BOOL(s.numReleaseRef == 0);
    EZ_TEST_BOOL(s.numRelease == 0);

    // Should make a copy and release it again.
    takeByValue(p);
    EZ_TEST_BOOL(p.pRefCounted == &s);
    EZ_TEST_BOOL(s.numAddRef == 2);
    EZ_TEST_BOOL(s.numReleaseRef == 1);
    EZ_TEST_BOOL(s.numRelease == 0);

    // Should move 'ownership' to the function, which releases its reference,
    // releasing all existing references in our case.
    takeByValue(move(p));
    EZ_TEST_BOOL(p.pRefCounted == nullptr);
    EZ_TEST_BOOL(s.numAddRef == 2);
    EZ_TEST_BOOL(s.numReleaseRef == 2);
    EZ_TEST_BOOL(s.numRelease == 1);
  }
}
