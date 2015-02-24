#pragma once

#include <Foundation/Threading/AtomicUtils.h>

namespace kr
{
  namespace RefCountedReleasePolicies
  {
    /// \brief Invalid policy. Useful for abstract base classes.
    struct Invalid
    {
      /// \note This function is intentionally not implemented and
      ///       will raise a linker error if used.
      template<typename T>
      static void release(T*&);
    };

    /// \brief Does nothing.
    struct DoNothing
    {
      template<typename T>
      static void release(T*&) {}
    };

    /// \brief Sets a pointer to a reference counted object to \a nullptr when released.
    struct SetNull
    {
      template<typename T>
      static void release(T*& pRefCounted)
      {
        pRefCounted = nullptr;
      }
    };

    /// \brief Calls EZ_DEFAULT_DELETE on a reference counted object when released.
    struct DefaultDelete
    {
      template<typename T>
      static void release(T*& pRefCounted)
      {
        if (pRefCounted)
          EZ_DEFAULT_DELETE(pRefCounted);
      }
    };
  }

  /// \brief Base class for reference counting.
  class RefCounted
  {
  public:
    // Default release policy.
    using ReleasePolicy = RefCountedReleasePolicies::SetNull;

  private: // *** Data
    /// \brief The current reference count.
    ezInt32 m_refCount = 0;

  public: // *** Construction
    /// \brief Default Constructor
    RefCounted() = default;

    RefCounted(const RefCounted&)
    {
      // Do not copy the ref count.
    }

    void operator=(const RefCounted&) // [tested]
    {
      // Do not copy the ref count.
    }

  public: // *** Accessors/Mutators
    /// \remark You should use free functions instead of calling this directly.
    ezInt32 refCountOf() const
    {
      return m_refCount;
    }

    /// \remark You should use free functions instead of calling this directly.
    void addRef()
    {
      ezAtomicUtils::Increment(m_refCount);
    }

    /// \remark You should use free functions instead of calling this directly.
    void releaseRef()
    {
      ezAtomicUtils::Decrement(m_refCount);
    }
  };

  /// \brief Returns the current reference count of \a pObj.
  /// \pre pObj must not be \a nullptr.
  template<typename T>
  ezInt32 refCountOf(const T* pObj)
  {
    EZ_ASSERT(pObj != nullptr, "Invalid object.");
    return pObj->refCountOf();
  }

  /// \brief Whether \a pObj is still referenced or not.
  /// \pre pObj must not be \a nullptr.
  template<typename T>
  bool isReferenced(const T* pObj)
  {
    return refCountOf(pObj) > 0;
  }

  /// \brief Adds a reference to \a pObj.
  /// \note Overload this function for special behavior.
  /// \pre pObj must not be \a nullptr.
  template<typename T>
  void addRef(T* pObj)
  {
    EZ_ASSERT(pObj != nullptr, "Invalid object.");
    pObj->addRef();
  }

  /// \brief Removes a reference from \a pObj and sets it to \a nullptr.
  /// \pre pObj must not be \a nullptr.
  template<typename T>
  void releaseRef(T*& pObj)
  {
    EZ_ASSERT(pObj != nullptr, "Invalid object.");

    pObj->releaseRef();
    if (!isReferenced(pObj))
      T::ReleasePolicy::release(pObj);
    pObj = nullptr;
  }

  /// \brief If \a pObj is a valid pointer, will call \c addRef(pObj).
  template <typename T>
  ezResult addRefIfValid(T* pObj)
  {
    if (pObj)
    {
      addRef(pObj);
      return EZ_SUCCESS;
    }
    return EZ_FAILURE;
  }

  /// \brief If \a pObj is a valid pointer, will call \c releaseRef(pObj).
  template<typename T>
  ezResult releaseRefIfValid(T*& pObj)
  {
    if (pObj)
    {
      releaseRef(pObj);
      return EZ_SUCCESS;
    }
    return EZ_FAILURE;
  }
}
