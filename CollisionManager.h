#include <SFML/Graphics.hpp>
#include <iostream>


class CollisionManager {
public:
    CollisionManager() {
        clock.restart();
    }
    bool rocket_floor(Rocket& r, Floor& f) {
        if (r.position.y - 50 < 0) { 
            std::cout << clock.getElapsedTime().asSeconds() << std::endl;
            clock.restart();
            return true;
        }
        return false;
    }
private:
    sf::Clock clock;
};