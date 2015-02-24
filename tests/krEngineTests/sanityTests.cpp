
// Check static 'inheritance' of type information
// ==============================================

namespace
{
  struct Original
  {
    enum { Value = 1 };
  };

  struct Plain : public Original
  {
  };

  struct Custom : public Original
  {
    enum { Value = 2 };
  };

  struct IndirectOriginal
  {
    using Type = Original;
  };

  struct IndirectPlain
  {
    using Type = Plain;
  };

  struct IndirectCustom
  {
    using Type = Custom;
  };
}

EZ_CHECK_AT_COMPILETIME(Plain::Value == Original::Value);
EZ_CHECK_AT_COMPILETIME(Custom::Value != Original::Value);

EZ_CHECK_AT_COMPILETIME(IndirectOriginal::Type::Value == Original::Value);
EZ_CHECK_AT_COMPILETIME(IndirectPlain::Type::Value == Original::Value);
EZ_CHECK_AT_COMPILETIME(IndirectCustom::Type::Value != Original::Value);
