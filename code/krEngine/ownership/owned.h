#pragma once

#include <krEngine/common/traits.h>
#include <krEngine/ownership/ownershipData.h>
#include <krEngine/ownership/borrowed.h>

namespace kr
{
  /// \brief Wrapper for ownership over an instance of an object.
  template<typename T>
  struct Owned
  {
  public: // *** Types
    using ElementType = NonRef<T>;
    using Data        = OwnershipData<T>;
    using Ptr         = ElementType*;
    using PtrToConst  = const ElementType*;
    using Ref         = ElementType&;
    using RefToConst  = const ElementType&;
    using CleanUp     = ezDelegate<void(Ptr)>;

  public: // *** Runtime
    Owned() = default;

    Owned(std::nullptr_t) : Owned() {}

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

    void operator =(std::nullptr_t) { this->reset(); }

    /// \pre Neither this nor \a other can be referenced (borrowed).
    void operator =(Owned&& other)
    {
      other.ensureNotReferenced();
      this->reset();
      this->cleanUp = move(other.cleanUp);
      this->data.ptr = other.yieldOwnership();
    }

    /// Free the internal object if any, and take ownership over \a newPtr.
    /// \param newPtr May not be a nullptr.
    /// \param newCleanUp Takes care of deleting \a newPtr.
    /// \pre newPtr is a different ptr than we already have.
    /// \pre This object is not referenced (borrowed).
    /// \sa reset()
    void assign(Ptr newPtr, CleanUp newCleanUp)
    {
      this->reset();
      this->data.ptr = newPtr;
      this->cleanUp = move(newCleanUp);
    }

    void reset()
    {
      if (*this != nullptr)
      {
        this->ensureNotReferenced();
        this->cleanUp(this->yieldOwnership());
        this->data = Data();
      }
    }

    /// Release ownership of the internal object and return the ptr to it.
    /// \note Does not invoke \c cleanUp() on the ptr.
    /// \return The ptr we owned.
    Ptr yieldOwnership()
    {
      auto ptr = this->data.ptr;
      this->data = Data();
      return ptr;
    }

    Ref operator *()
    {
      this->validate();
      return *this->data.ptr;
    }

    RefToConst operator *() const
    {
      this->validate();
      return *this->data.ptr;
    }

    /// Get the actual object.
    Ptr operator ->()
    {
      this->validate();
      return this->data.ptr;
    }

    PtrToConst operator ->() const
    {
      this->validate();
      return this->data.ptr;
    }

    operator Ptr()
    {
      return this->data.ptr;
    }

    operator PtrToConst() const
    {
      return this->data.ptr;
    }

    operator Borrowed<T>()
    {
      Borrowed<T> borrowed;
      borrowed = this->data;
      return move(borrowed);
    }

    operator Borrowed<const T>() const
    {
      Borrowed<const T> borrowed;
      // Add const to the ownership data.
      borrowed = reinterpret_cast<const OwnershipData<const T>&>(this->data);
      return move(borrowed);
    }

    void swap(Owned& rhs)
    {
      swap(this->data.ptr, rhs.data.ptr);
      swap(this->cleanUp, rhs.cleanUp);
    }

    bool operator ==(std::nullptr_t) const { return this->data.ptr == nullptr; }
    bool operator !=(std::nullptr_t) const { return !(*this == nullptr); }

  public: // *** Data
    Data data;
    CleanUp cleanUp;

  private: // *** Internal
    void ensureNotReferenced() const
    {
      EZ_ASSERT_ALWAYS(this->data.refCount == 0,
                       "Someone still has a reference to this object.");
    }

    void validate() const
    {
      EZ_ASSERT_ALWAYS(*this != nullptr,
                       "Validation failed: we have a nullptr.");
    }
  };

  /// Own a pointer.
  template<typename T, typename CleanUp>
  Owned<T> own(T* ptr, CleanUp cleanUp)
  {
    // Note: Checking ptr for validity is done in ctor of Owned<>.
    return Owned<T>(const_cast<kr::NonConst<T>*>(ptr), forward<CleanUp>(cleanUp));
  }

  /// Own a pointer.
  template<typename T>
  Owned<T> own(T* ptr, std::nullptr_t)
  {
    return own(ptr, [](T*){});
  }

  template<typename T>
  Borrowed<T> borrow(Owned<T>& owned)
  {
    return static_cast<Borrowed<T>>(owned);
  }

  template<typename T>
  Borrowed<const T> borrow(const Owned<T>& owned)
  {
    return static_cast<Borrowed<const T>>(owned);
  }
}
