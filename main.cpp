#include <SFML/Graphics.hpp>

#include "manager.h"

int main()
{
    Manager m {};
    sf::RenderWindow window(sf::VideoMode(m.get_window_width(), m.get_window_height()), "");
    window.setFramerateLimit(60);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        m.update(window);
        Settings::restartc();
        window.display();
    }
    return 0;
}