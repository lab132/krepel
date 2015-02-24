#pragma once

#include <krEngine/referenceCounting/refCounted.h>

#include <Foundation/Types/TypeTraits.h>

namespace kr
{
  /// \brief Takes care of increasing and decreasing references
  ///        to some instance of \a T.
  template<typename T>
  struct RefCountedPtr
  {
  public: // *** Types
    using PtrType = typename ezTypeTraits<T>::NonConstType;

  public: // *** Data
    /// \note You should not use this directly.
    PtrType* pRefCounted;

  public: // *** Construction
    RefCountedPtr() : pRefCounted(nullptr) {}

    /// \brief Construct from \a ptr.
    RefCountedPtr(T* ptr) :
      pRefCounted(const_cast<PtrType*>(ptr))
    {
      addRefIfValid(pRefCounted);
    }

    /// \brief Copy constructor.
    RefCountedPtr(const RefCountedPtr& other) :
      pRefCounted(other.pRefCounted)
    {
      addRefIfValid(pRefCounted);
    }

    /// \brief Move constructor.
    RefCountedPtr(RefCountedPtr&& other) :
      pRefCounted(move(other.pRefCounted))
    {
      other.pRefCounted = nullptr;
    }

    /// \brief Destructor.
    ~RefCountedPtr()
    {
      releaseRefIfValid(pRefCounted);
    }

  public: // *** Assignment
    /// \brief Copy assignment.
    void operator =(const RefCountedPtr& other)
    {
      // If \a other has the identical pointer, there's nothing to do here.
      if (other.pRefCounted == this->pRefCounted)
        return;

      RefCountedPtr cpy(other);
      *this = move(cpy);
    }

    /// \brief Move assignment.
    void operator =(RefCountedPtr&& other)
    {
      // Handle self-assignment.
      if (&other == this)
        return;

      releaseRefIfValid(pRefCounted);
      pRefCounted = move(other.pRefCounted);
      other.pRefCounted = nullptr;
    }

    /// \brief Pointer assignment.
    void operator =(T* p)
    {
      // If we already have this pointer, don't do anything.
      if (p == pRefCounted)
        return;

      auto ptr = const_cast<PtrType*>(p);
      addRefIfValid(ptr);
      releaseRefIfValid(pRefCounted);
      pRefCounted = ptr;
    }

  public: // *** Operators and implicit conversion
    operator T*()
    {
      EZ_ASSERT(pRefCounted != nullptr, "Invalid operation.");
      return pRefCounted;
    }

    T& operator *()
    {
      EZ_ASSERT(pRefCounted != nullptr, "Invalid operation.");
      return *pRefCounted;
    }

    const T& operator *() const
    {
      EZ_ASSERT(pRefCounted != nullptr, "Invalid operation.");
      return *pRefCounted;
    }

    T* operator ->()
    {
      EZ_ASSERT(pRefCounted != nullptr, "Invalid operation.");
      return pRefCounted;
    }

    const T* operator ->() const
    {
      EZ_ASSERT(pRefCounted != nullptr, "Invalid operation.");
      return pRefCounted;
    }

    /// \brief Validation check. Returns false if we hold a \a nullptr.
    operator bool() const
    {
      return pRefCounted != nullptr;
    }

    template<typename U>
    bool operator ==(const RefCountedPtr<U>& other)
    {
      return pRefCounted == other.pRefCounted;
    }

    template<typename U>
    bool operator !=(const RefCountedPtr<U>& other)
    {
      return !(*this == other);
    }

  public: // *** Accessors
    T* get() const { return pRefCounted; }
  };

  template<typename T>
  inline RefCountedPtr<T> makeRefCountedPtr(T* arg)
  {
    return RefCountedPtr<T>(arg);
  }

  template<typename T>
  inline bool isValid(const RefCountedPtr<T>& p)
  {
    return (bool)p;
  }

  template<typename T>
  inline bool isNull(const RefCountedPtr<T>& p)
  {
    return p.get() == nullptr;
  }

  template<typename T>
  inline void invalidate(const RefCountedPtr<T>& p)
  {
    p = nullptr;
  }
}
