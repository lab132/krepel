#include "application/pch.h"
#include <SFML/Audio.hpp>
#include <Foundation/Math/Vec3.h>
#include <Foundation/IO/OSFile.h>

#include <krEngine/rendering.h>

int main(int argc, const char* argv[])
{
  ezVec3 v(1, 2, 3);
  printf("Hello world! %f;%f;%f Press ESCAPE to quit.\n", v.x, v.y, v.z);

  // Use something that is exported by ezEngine
  printf("App dir: %s", ezOSFile::GetApplicationDirectory());
  printf("something?: %s", kr::getSomething());

  sf::RenderWindow window(sf::VideoMode(800, 600), "krepel application");

  kr::Line l;
  l.start.Set(1, 2);
  l.end.Set(3, 4);
  l.layer = 123;
  kr::DebugRenderer::draw(l);

  while(window.isOpen())
  {
    sf::Event event;
    while(window.pollEvent(event))
    {
      bool escapePressed = event.type == sf::Event::KeyReleased
                        && event.key.code == sf::Keyboard::Escape;
      if (escapePressed || event.type == sf::Event::Closed)
      {
        window.close();
      }
    }

    window.display();
  }

  return 0;
}
