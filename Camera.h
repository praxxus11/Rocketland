#include <SFML/Graphics.hpp>
#include <iostream>

class Camera {
public:
    Camera() :
        center(0, 0),
        left(0),
        right(0)
    {
    }
private:
    sf::Vector2i center;
    int left;
    int right;
};