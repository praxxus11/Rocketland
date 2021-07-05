#include <SFML/Graphics.hpp>
#include <iostream>


class CollisionManager {
public:
    CollisionManager() {
        clock.restart();
    }
    bool rocket_floor_collision(Rocket& r, Floor& f) {
        return r.getGlobalBounds().intersects(f.getGlobalBounds());
    }
private:
    sf::Clock clock;
};