#pragma once

template<typename T, typename Derived>
ezArrayPtr<T> ezMakeArrayPtr(ezArrayBase<T, Derived>& arr)
{
  // ezArrayBase defines an implicit conversion operator to ezArrayPtr.
  return static_cast< ezArrayPtr<T> >(arr);
}
