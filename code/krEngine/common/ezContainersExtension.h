#pragma once

template<typename T, typename Derived>
T* begin(ezArrayBase<T, Derived>& arr)
{
  return arr.GetData();
}

template<typename T, typename Derived>
const T* begin(const ezArrayBase<T, Derived>& arr)
{
  return arr.GetData();
}

template<typename T, typename Derived>
T* end(ezArrayBase<T, Derived>& arr)
{
  return arr.GetData() + arr.GetCount();
}

template<typename T, typename Derived>
const T* end(const ezArrayBase<T, Derived>& arr)
{
  return arr.GetData() + arr.GetCount();
}
