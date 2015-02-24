#pragma once

template<typename T>
ezArrayPtr<T> ezMakeArrayPtr(T* data, ezUInt32 count)
{
  return ezArrayPtr<T>(data, count);
}

template<typename T, size_t N>
ezArrayPtr<T> ezMakeArrayPtr(T (&staticArray)[N])
{
  return ezArrayPtr<T>(staticArray);
}

template<typename T, typename Derived>
ezArrayPtr<T> ezMakeArrayPtr(ezArrayBase<T, Derived>& arr)
{
  // ezArrayBase defines an implicit conversion operator to ezArrayPtr.
  return (ezArrayPtr<T>)arr;
}

template<typename T>
T* begin(ezArrayPtr<T> arrayPtr)
{
  return arrayPtr.GetPtr();
}

template<typename T>
T* end(ezArrayPtr<T> arrayPtr)
{
  return arrayPtr.GetPtr() + arrayPtr.GetCount();
}
