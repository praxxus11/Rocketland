#include <SFML/Graphics.hpp>

#include "manager.h"
#include "Camera.h"

int main()
{
    srand(time(nullptr));
    sf::ContextSettings settings;
    settings.antialiasingLevel= 8;

    Manager m {};
    Camera c {};
    sf::RenderWindow window(sf::VideoMode(m.get_window_width(), m.get_window_height()), "", sf::Style::Default, settings);
    window.setFramerateLimit(10);
    window.setPosition(sf::Vector2i(1920-m.get_window_width(), 0));
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }
        window.clear();
        m.update();
        c.update_from_rocket(m.get_rocket_pos());
        m.draw(window);
        Env::restartc();
        window.display();
    }
    return 0;
}