#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

class Rocket : public sf::Drawable, public sf::Transformable {
public:
    Rocket() : rotation(0.f),
        pixels_tall(1120),
        scale(50.f * Settings::pixpmeter / pixels_tall, 50.f * Settings::pixpmeter / pixels_tall),
        position(0, 300),
        vel(0, 0),
        accel(0, 0)
    {
        if (!texture.loadFromFile("imgs/ship.png")) {
            std::cout << "Rocket png not loaded";
        }
        texture.setSmooth(true);
        sprite.setTexture(texture);
    }
    int pixels_tall; // change this code later
    float rotation;
    sf::Vector2f scale;
    sf::Vector2f position; 
    sf::Vector2f vel;
    sf::Vector2f accel;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        sf::FloatRect bb = sprite.getGlobalBounds();
        states.transform *= getTransform();
        target.draw(sprite, states);
        sf::RectangleShape shp (sf::Vector2f(bb.width, bb.height));
        shp.setFillColor(sf::Color::Transparent);
        shp.setOutlineColor(sf::Color::Red);
        shp.setOutlineThickness(9.f);
        target.draw(shp, states);
    }
    sf::Texture texture;
    sf::Sprite sprite;
};