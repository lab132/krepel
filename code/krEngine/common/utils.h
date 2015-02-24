#pragma once

namespace kr
{
  template<typename T>
  bool isNull(T* ptr) { return ptr == nullptr; }

  template<typename T>
  bool isValid(T* ptr) { return !isNull(ptr); }

  template<typename T>
  inline void invalidate(T*& ptr) { ptr = nullptr; }
}
