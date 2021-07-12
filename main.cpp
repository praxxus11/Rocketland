#include <SFML/Graphics.hpp>

#include "manager.h"

int main()
{
    srand(time(nullptr));
    int ct = 0;

    Manager m {};
    sf::RenderWindow window(sf::VideoMode(m.get_window_width(), m.get_window_height()), "");
    window.setFramerateLimit(30);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (ct++%3==0) 
            std::cout << 1/Env::g_elapsed() << '\n';

        window.clear();
        m.update(window);
        Env::restartc();
        window.display();
    }
    return 0;
}