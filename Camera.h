#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

class Camera {
public:
    Camera() {}
    void update() const {
        const int move_amt = 300;
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
        update_zoom();
    }
    void update_from_rocket(const sf::Vector2f& pos) const {
        Env::camera_pos = pos;
        update_zoom();
    }
    void update_zoom() const {
        const float zoom_amt = 5;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Equal)) {
            Env::pixpmeter += Env::g_elapsed() * zoom_amt;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Hyphen)) {
            Env::pixpmeter = std::max(0.1f, Env::pixpmeter - Env::g_elapsed() * zoom_amt);
        }
    }
private:
};