#include "application/pch.h"
#include <SFML/Audio.hpp>

int main(int argc, const char* argv[])
{
    printf("Hello world! Press ESCAPE to quit.\n");

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
