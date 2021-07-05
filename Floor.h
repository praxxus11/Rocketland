#include <SFML/Graphics.hpp>
#include <iostream>

class Floor : public sf::Drawable, public sf::Transformable {
public:
    Floor() {
        rect.setPosition(0, Settings::ww - Settings::floor_hei);
        rect.setFillColor(sf::Color(0,100,0));
        rect.setSize(sf::Vector2f(Settings::ww, Settings::floor_hei));
    }
    sf::FloatRect getGlobalBounds() const {
        return getTransform().transformRect(rect.getGlobalBounds());
    }
private:
    sf::RectangleShape rect;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(rect);
        sf::FloatRect bb = this->getGlobalBounds();
        sf::RectangleShape shp (sf::Vector2f(bb.width, bb.height));
        shp.setPosition(rect.getPosition());
        shp.setFillColor(sf::Color::Transparent);
        shp.setOutlineColor(sf::Color::Red);
        shp.setOutlineThickness(2.f);
        target.draw(shp, states);
    }
};