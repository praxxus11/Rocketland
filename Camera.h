#include <SFML/Graphics.hpp>
#include <iostream>

class Camera {
public:
    Camera() :
        center(0, 0)
    {
    }
    void update() const {
        const int move_amt = 100;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            Env::camera_pos.y += Env::g_elapsed() * move_amt;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            Env::camera_pos.y -= Env::g_elapsed() * move_amt;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            Env::camera_pos.x += Env::g_elapsed() * move_amt;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            Env::camera_pos.x -= Env::g_elapsed() * move_amt; 
        }
    }
    void update_from_rocket(const sf::Vector2f& pos) {
        Env::camera_pos = pos;
    }
private:
    sf::Vector2i center; // (meters, meteres)
};