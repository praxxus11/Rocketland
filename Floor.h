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
        sf::FloatRect ir = rect.getGlobalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Settings::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Settings::pixpmeter, ir.height/Settings::pixpmeter);
    }
private:
    sf::RectangleShape rect;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        target.draw(rect);
        sf::FloatRect bb = getGlobalBounds();
        sf::RectangleShape rct(Settings::convertSize(sf::Vector2f(bb.width, bb.height)));
        // sf::RectangleShape rct(sf::Vector2f(bb.width, bb.height));
        rct.setFillColor(sf::Color::Transparent);
        rct.setOutlineColor(sf::Color::Red);
        rct.setOutlineThickness(3.f);
        sf::Vector2f temp = Settings::metersToPixels(sf::Vector2f(bb.left, bb.top));
        rct.setPosition(temp.x, temp.y);
        // rct.setPosition(bb.left, bb.top-bb.height);
        target.draw(rct);

        // sf::RectangleShape aa(sf::Vector2f(100, 100));
        // aa.setFillColor(sf::Color::Blue);
        // aa.setPosition(bb.left, bb.top);
        // target.draw(aa);
    }
};