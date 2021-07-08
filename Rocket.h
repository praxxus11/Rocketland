#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

class Rocket : public sf::Drawable, public sf::Transformable {
public:
    Rocket() : rotation(0.f),
        pixels_tall(1120),
        scale(50.f * Settings::pixpmeter / pixels_tall, 50.f * Settings::pixpmeter / pixels_tall),
        position(0, 100),
        vel(0, 0),
        accel(0, 0),
        explosion_anim(128, 3328, 26, 3)
    {
        if (!texture.loadFromFile("imgs/ship.png")) {
            std::cout << "Rocket png not loaded";
        }
        texture.setSmooth(true);
        sprite.setTexture(texture);
        explosion_anim.setScale(2,2);
    }
    sf::FloatRect getGlobalBounds() const {
        return getTransform().transformRect(sprite.getGlobalBounds());
    }
    void explode() {
        std::cout << "Called";
    }
    int pixels_tall; // change this code later
    float rotation;
    sf::Vector2f scale;
    sf::Vector2f position; 
    sf::Vector2f vel;
    sf::Vector2f accel;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(sprite, states);
    }
    sf::Texture texture;
    sf::Sprite sprite;
    GIFhandler explosion_anim;
};