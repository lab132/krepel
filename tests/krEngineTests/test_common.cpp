
EZ_CREATE_SIMPLE_TEST_GROUP(Common);

EZ_CREATE_SIMPLE_TEST(Common, ezArrayPtrExtension)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Make From Ptr + Size")
  {
    int data[3] = { 3, 5, 7 };
    int* pData = data;
    int count = 3;

    auto arrayPtr = ezMakeArrayPtr(pData, count);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Make From Plain Array")
  {
    int data[3] = { 3, 5, 7 };

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Make From ezStaticArray")
  {
    ezStaticArray<int, 3> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Make From ezHybridArray")
  {
    ezHybridArray<int, 2> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Make From ezDynamicArray")
  {
    ezDynamicArray<int> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    auto arrayPtr = ezMakeArrayPtr(data);

    ezStaticArray<int, 3> result;
    for (auto i : arrayPtr)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }
}

EZ_CREATE_SIMPLE_TEST(Common, ezContainerExtension)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezStaticArray")
  {
    ezStaticArray<int, 3> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    ezStaticArray<int, 3> result;
    for (auto i : data)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezDynamicArray")
  {
    ezDynamicArray<int> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    ezStaticArray<int, 3> result;
    for (auto i : data)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezHybridArray")
  {
    ezHybridArray<int, 2> data;
    data.ExpandAndGetRef() = 3;
    data.ExpandAndGetRef() = 5;
    data.ExpandAndGetRef() = 7;

    ezStaticArray<int, 3> result;
    for (auto i : data)
    {
      result.PushBack(i);
    }

    EZ_TEST_INT(result[0], 3);
    EZ_TEST_INT(result[1], 5);
    EZ_TEST_INT(result[2], 7);
  }

  EZ_TEST_BLOCK(ezTestBlock::Disabled, "ezMap")
  {
    KR_NOT_IMPLEMENTED;
  }

  EZ_TEST_BLOCK(ezTestBlock::Disabled, "ezHashTable")
  {
    KR_NOT_IMPLEMENTED;
  }
}

EZ_CREATE_SIMPLE_TEST(Common, Utils)
{
  using namespace kr;

  int data = 42;
  auto pData = &data;

  EZ_TEST_BOOL(isValid(pData));
  EZ_TEST_BOOL(!isNull(pData));

  invalidate(pData);
  EZ_TEST_BOOL(!isValid(pData));
  EZ_TEST_BOOL(isNull(pData));
}
