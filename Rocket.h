#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

class Rocket : public sf::Drawable, public sf::Transformable {
public:
    Rocket() : rotation(0.f),
        scale(0.3, 0.3),
        position(20, (Settings::wh - Settings::floor_hei) / Settings::pixpmeter - 100),
        vel(0, 0),
        accel(0, 0)
    {
        if (!texture.loadFromFile("imgs/ship.png")) {
            std::cout << "Rocket png not loaded";
        }
        texture.setSmooth(true);
        sprite.setTexture(texture);
    }
    float rotation;
    std::pair<float, float> scale;
    std::pair<float, float> position; 
    std::pair<float, float> vel;
    std::pair<float, float> accel;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        sf::FloatRect bb = sprite.getGlobalBounds();
        states.transform *= getTransform();
        target.draw(sprite, states);
        sf::RectangleShape shp (sf::Vector2f(bb.width, bb.height));
        shp.setPosition(bb.left, bb.top);
        shp.setFillColor(sf::Color::Transparent);
        shp.setOutlineColor(sf::Color::Red);
        shp.setOutlineThickness(9.f);
        target.draw(shp, states);
    }
    sf::Texture texture;
    sf::Sprite sprite;
};