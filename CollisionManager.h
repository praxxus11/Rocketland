#include <SFML/Graphics.hpp>
#include <iostream>


class CollisionManager {
public:
    CollisionManager() 
    {
    }
    ~CollisionManager()
    {
    }
    bool rocket_floor_collision(const Rocket& r, const  Floor& f) {
        return r.getGlobalBounds().intersects(f.getGlobalBounds());
    }
private:
};