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
