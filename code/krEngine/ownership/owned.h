#pragma once

#include <Foundation/Threading/AtomicUtils.h>
#include <krEngine/common/traits.h>

namespace kr
{
  template<typename T>
  struct OwnershipData
  {
    T* ptr = nullptr;
    mutable ezInt64 refCount = 0;
  };

  template<typename T>
  void addRef(const OwnershipData<T>& data) { ezAtomicUtils::Increment(data.refCount); }

  template<typename T>
  void removeRef(const OwnershipData<T>& data) { ezAtomicUtils::Decrement(data.refCount); }

  /// \brief Wrapper for ownership over an instance of an object.
  template<typename T>
  struct Owned
  {
    using ElementType = NonRef<T>;
    using Data        = OwnershipData<T>;
    using Ptr         = ElementType*;
    using PtrToConst  = const ElementType*;
    using Ref         = ElementType&;
    using RefToConst  = const ElementType&;
    using CleanUp     = ezDelegate<void(Ptr)>;


    Owned() = default;

    /// Take ownership.
    Owned(Ptr p, CleanUp del) : cleanUp(move(del))
    {
      this->data.ptr = p;
      validate();
    }

    /// 'Steal' ownership from another wrapper.
    template<typename U>
    Owned(Owned<U>&& other) : cleanUp(move(other.cleanUp))
    {
      this->data.ptr = other.yieldOwnership();
    }

    ~Owned() { this->reset(); }

    /// Cannot copy an owned ptr.
    Owned(const Owned& other) = delete;

    /// Cannot copy an owned ptr.
    void operator =(Owned& other) = delete;

    /// Assignment is disallowed. Use \a reset(ptr) to be explicit.

    /// Free the internal object if any, and take ownership over \a newPtr.
    /// \param newPtr May not be a nullptr.
    /// \param newCleanUp Takes care of deleting \a newPtr.
    /// \pre newPtr is a different ptr than we already have.
    /// \sa reset()
    void assign(Ptr newPtr, CleanUp newCleanUp)
    {
      this->reset();
      this->data.ptr = newPtr;
      this->cleanUp = newCleanUp;
    }

    void reset()
    {
      if (this->isValid())
      {
        EZ_ASSERT_DEBUG(this->data.refCount <= 0, "Object is still referenced!");
        this->cleanUp(this->yieldOwnership());
      }
    }

    /// Release ownership of the internal object and return the ptr to it.
    /// \note Does not invoke the cleanUp on the ptr.
    /// \return The ptr we owned.
    Ptr yieldOwnership()
    {
      Ptr oldPtr = nullptr;
      kr::swap(this->data.ptr, oldPtr);
      return oldPtr;
    }

    /// Get the actual object.
    Ref ref()
    {
      this->validate();
      return *this->data.ptr;
    }

    RefToConst ref() const
    {
      this->validate();
      return *this->data.ptr;
    }

    void swap(Owned& rhs)
    {
      swap(this->data.ptr, rhs.data.ptr);
      swap(this->cleanUp, rhs.cleanUp);
    }

    bool isValid() const { return this->data.ptr != nullptr; }

    operator bool() const { return this->valid(); }

    /// \note Internal.
    void validate() const
    {
      EZ_ASSERT_ALWAYS(this->isValid(), "Validation failed: we have a nullptr.");
    }

    Data data;
    CleanUp cleanUp;
  };

  /// Own a pointer.
  template<typename T, typename CleanUp>
  Owned<T> own(T* ptr, CleanUp cleanUp)
  {
    // Note: Checking ptr for validity is done in ctor of Owned<>.
    return Owned<T>(const_cast<kr::NonConst<T>*>(ptr), forward<CleanUp>(cleanUp));
  }
}
