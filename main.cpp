#include <SFML/Graphics.hpp>

#include "manager.h"
#include "Camera.h"

int main()
{
    srand(time(nullptr));
    sf::ContextSettings settings;
    settings.antialiasingLevel= 8;

    Manager m {};
    sf::RenderWindow window(sf::VideoMode(m.get_window_width(), m.get_window_height()), "", sf::Style::Default, settings);
    window.setFramerateLimit(120);
    window.setPosition(sf::Vector2i(1920-m.get_window_width(), 0));
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }
        window.clear();
        m.update(window);
        Env::restartc();
        window.display();
    }
    return 0;
}