#pragma once

#include <Foundation/Threading/AtomicUtils.h>

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
}
