#include <SFML/Graphics.hpp>

#include "Preprocessing.h"
#include "manager.h"

#include "Camera.h"

int main()
{
    srand(time(nullptr));
    Manager m {};
    
    sf::ContextSettings settings;
    settings.antialiasingLevel= 8;
    const bool show = 1;
    if (!show) {
        while (1) {
            m.update();
            Env::restartc();
        }
    }
    else {
        Camera c {};
        sf::RenderWindow window(sf::VideoMode(m.get_window_width(), m.get_window_height()), "", sf::Style::Default, settings);
        window.setFramerateLimit(40);
        window.setPosition(sf::Vector2i(1920-m.get_window_width(), 0));
        sf::RectangleShape sky_blue(sf::Vector2f(float(window.getSize().x), float(window.getSize().y))); // color to reset board
        sky_blue.setFillColor(sf::Color(135, 206, 235));
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                    window.close();
            }
            window.clear();
            window.draw(sky_blue);
            m.update();
            // c.update();
            c.update_from_rocket(m.get_rocket_pos());
            m.draw(window);
            Env::restartc();
            window.display();
        }
    }
    return 0;
}