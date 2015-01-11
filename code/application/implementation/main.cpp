#include "application/pch.h"
#include <SFML/Audio.hpp>
#include <Foundation/Math/Vec3.h>
#include <Foundation/IO/OSFile.h>

int main(int argc, const char* argv[])
{
    ezVec3 v(1, 2, 3);
    printf("Hello world! %f;%f;%f Press ESCAPE to quit.\n", v.x, v.y, v.z);

    // Use something that is exported by ezEngine
    printf("App dir: %s", ezOSFile::GetApplicationDirectory());
    printf("something?: %s", kr::getSomething());

    sf::Window app(sf::VideoMode(800, 600), "krepel application");

    while(app.isOpen())
    {
        sf::Event event;
        while(app.pollEvent(event))
        {
            bool escapePressed = event.type == sf::Event::KeyReleased
                              && event.key.code == sf::Keyboard::Escape;
            if (escapePressed || event.type == sf::Event::Closed)
            {
                app.close();
            }
        }
        app.display();
    }

    return 0;
}
