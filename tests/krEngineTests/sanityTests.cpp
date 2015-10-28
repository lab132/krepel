// Note: Some of the checks in this tests might seem trivial or
//       the concepts under test might even be standardized,
//       but we check for their validity so we can be sure things work as expected.

// Check static 'inheritance' of type information
// ==============================================
#include <krEngineTests/pch.h>

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

EZ_CHECK_AT_COMPILETIME(Original::Value == 1);
EZ_CHECK_AT_COMPILETIME(Plain::Value == 1);
EZ_CHECK_AT_COMPILETIME(Custom::Value == 2);

EZ_CHECK_AT_COMPILETIME(IndirectOriginal::Type::Value == 1);
EZ_CHECK_AT_COMPILETIME(IndirectPlain::Type::Value == 1);
EZ_CHECK_AT_COMPILETIME(IndirectCustom::Type::Value == 2);

// Some OpenGL Stuff
// =================

EZ_CHECK_AT_COMPILETIME(GL_TEXTURE1 == GL_TEXTURE0 + 1);
EZ_CHECK_AT_COMPILETIME(GL_TEXTURE2 == GL_TEXTURE1 + 1);
EZ_CHECK_AT_COMPILETIME(GL_TEXTURE3 == GL_TEXTURE2 + 1);
EZ_CHECK_AT_COMPILETIME(GL_TEXTURE4 == GL_TEXTURE3 + 1);
