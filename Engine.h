#include <SFML/Graphics.hpp>
#include <iostream>

#include "Flame.h"

class Engine : public GameObjectRelative {
public:
    Engine(sf::Vector2f pos, GameObject& parent) :
        GameObjectRelative(pos, parent) 
    {
        rect.setFillColor(sf::Color(255, 120, 0));
    }
    Engine(const Engine& e) :
        GameObjectRelative(e.position, e.parent)
    {
        rect.setFillColor(e.rect.getFillColor());
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = rect.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update(float scale) {
        irlSetDisplacement(sf::Vector2f(3/scale, 50/scale));
        rect.setSize(Env::convertSize(sf::Vector2f(5/scale, (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ? (rand()%7)+10 : 10) /scale)));
        float sc = (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) ? (rand()%100)/300 + 1 : 1;
        rect.setScale(sc, sc);
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(rect, states);
    }
    sf::RectangleShape rect;
    float angular_delta; // -90 -> 90 degress, where 0 degrees is downward
};
