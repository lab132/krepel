#include <krEngine/rendering/sprite.h>

void kr::update(Sprite& sprite)
{
  sprite.m_needsUpdate = false;
  // TODO Implement me.
}

void kr::draw(Sprite& sprite)
{
  if (sprite.m_needsUpdate)
    update(sprite);

  // TODO Implement me.
}
