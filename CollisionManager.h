#include <SFML/Graphics.hpp>
#include <iostream>


class CollisionManager {
public:
    CollisionManager() {
        clock.restart();
    }
    bool rocket_floor_collision(Rocket& r, Floor& f) {
        bool col = r.getGlobalBounds().intersects(f.getGlobalBounds());
        return col;
    }
private:
    sf::Clock clock;
    bool played = 0;
};