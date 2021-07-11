#include <SFML/Graphics.hpp>
#include <iostream>

class Floor : public GameObject {
public:
    Floor() :
        GameObject({-Settings::ww/(2*Settings::pixpmeter), 0}, {1, 1}, 0) 
    {
        rect.setFillColor(sf::Color(0,100,0));
        rect.setSize(sf::Vector2f(Settings::ww, Settings::floor_hei));
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = rect.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Settings::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Settings::pixpmeter, ir.height/Settings::pixpmeter);
    }
private:
    sf::RectangleShape rect;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(rect, states);
    }
};