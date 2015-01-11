#pragma once

namespace kr
{
  struct DrawableBase
  {
    ezInt32 layer = 0;
    ezColor color = ezColor::GetWhite();
  };
}
