#pragma once
#include <krEngine/rendering/drawableBase.h>

namespace kr
{
  struct Line : public DrawableBase
  {
    ezVec2 start;
    ezVec2 end;
  };

  struct Circle : public DrawableBase
  {
    ezVec2 center;
    float radius;
  };

  class KR_ENGINE_API DebugRenderer
  {
  public:
    static void draw(const Line& line);
    static void draw(const Circle& circle);
  protected:
  private:
  };
}
