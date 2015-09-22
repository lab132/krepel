#pragma once
#include <krEngine/ownership/owned.h>

namespace kr
{
  template<typename T>
  struct Borrowed
  {
    using ElementType = NonRef<T>;
    using Data        = OwnershipData<T>;
    using Ptr         = ElementType*;
    using PtrToConst  = const ElementType*;
    using Ref         = ElementType&;
    using RefToConst  = const ElementType&;


    Borrowed() = default;
    Borrowed(Data&&) = delete;
    void operator =(Data&&) = delete;

    template<typename U>
    Borrowed(const Borrowed<U>& other) : pData(other.pData)
    {
      this->addRefIfValid();
    }

    Borrowed(const Data& newData) : pData(&newData)
    {
      this->addRefIfValid();
    }

    template<typename U>
    void operator =(const Borrowed<U>& other)
    {
      *this = *other.pData;
    }

    ~Borrowed()
    {
      this->removeRefIfValid();
      this->pData = nullptr;
    }

    void operator =(const Data& newData)
    {
      addRef(newData);
      this->removeRefIfValid();
      this->pData = &newData;
    }

    /// Get the actual object.
    Ref ref()
    {
      this->validate();
      return *this->pData->ptr;
    }

    RefToConst ref() const
    {
      this->validate();
      return *this->pData->ptr;
    }

    operator Ptr()
    {
      this->validateData();
      return this->pData->ptr;
    }

    operator PtrToConst() const
    {
      this->validateData();
      return this->pData->ptr;
    }

    bool isValid() const
    {
      this->validateData();
      return this->pData->ptr != nullptr;
    }

    operator bool() const { return this->valid(); }

    /// \note Internal.
    inline void validateData() const
    {
      EZ_ASSERT_ALWAYS(this->pData != nullptr, "Invalid state: Internal data ptr is null.");
    }

    /// \note Internal.
    inline void validate() const
    {
      this->validateData();
      EZ_ASSERT_ALWAYS(this->pData->ptr != nullptr, "Validation failed: we have a nullptr.");
    }

    void addRefIfValid()
    {
      if (this->isValid())
      {
        this->validateData();
        addRef(*this->pData);
      }
    }

    void removeRefIfValid()
    {
      if (this->isValid())
      {
        this->validateData();
        removeRef(*this->pData);
      }
    }

    const Data* pData;
  };

  template<typename T>
  Borrowed<T> borrow(Owned<T>& owned)
  {
    return Borrowed<T>(owned.data);
  }
}
