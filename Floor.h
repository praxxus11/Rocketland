#include <SFML/Graphics.hpp>
#include <iostream>

// this is not necessary: only to make visual studio code not give an error
// #include "Settings.h"

class Floor : public sf::Drawable, public sf::Transformable {
public:
    Floor() {
        rect.setPosition(0, Settings::ww - Settings::floor_hei);
        rect.setFillColor(sf::Color(0,100,0));
        rect.setSize(sf::Vector2f(Settings::ww, Settings::floor_hei));
    }
private:
    sf::RectangleShape rect;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(rect);
    }
};