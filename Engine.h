#include <SFML/Graphics.hpp>
#include <iostream>

#include "Flame.h"

class Engine : public GameObjectRelative {
public:
    Engine(sf::Vector2f pos, GameObject& parent) :
        GameObjectRelative(pos, parent),
        angular_delta(0) 
    {
        rect.setFillColor(sf::Color(255, 120, 0));
        rect.setSize(sf::Vector2f(100, 500));
        rect.setOrigin(50, 0);

        base.setFillColor(sf::Color(200,200,200));
        base.setSize(sf::Vector2f(150, 100));
        base.setOrigin(75, 0);
    }
    Engine(const Engine& e) :
        GameObjectRelative(e.position, e.parent)
    {
        rect.setFillColor(e.rect.getFillColor());
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = rect.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent.getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        const float scale = parent.getScale().x;
        irlSetDisplacement(sf::Vector2f(5/scale, 48/scale));
        float sc = (rand()%100)/200. + 1;
        setScale(1, sc);
        setRotation(angular_delta);
        engine_on = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angular_delta = std::max(-45.f, angular_delta-100*Env::g_elapsed());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angular_delta = std::min(45.f, angular_delta+100*Env::g_elapsed());
        }
        else { 
            angular_delta -= 5*angular_delta*Env::g_elapsed();
        }
    }
    float get_angle() const {
        return angular_delta;
    }
    float is_engine_on() const {
        return engine_on;
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // take note that the order in which the transforms are applied DOES matter
        states.transform *= parent.getTransform() * getTransform(); 
        if (is_engine_on())
            target.draw(rect, states);
        target.draw(base, states);
    }
    sf::RectangleShape rect;
    sf::RectangleShape base;
    float angular_delta; // -90 -> 90 degress, where 0 degrees is downward
    bool engine_on;
};
