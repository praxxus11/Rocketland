#include <SFML/Graphics.hpp>
#include <iostream>

#include "Flame.h"

class Engine : public GameObject {
public:
    Engine() :
        GameObject({0, 0}, {1, 1}, 0)
    {
        rect.setSize(sf::Vector2f(200, 200)); 
        rect.setFillColor(sf::Color::White);
    }
    Engine(const Engine& e) :
        GameObject(e.position, e.scale, e.rotation)
    {
        rect.setSize(e.rect.getSize());
        rect.setFillColor(e.rect.getFillColor());
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = rect.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(rect, states);
    }
    sf::RectangleShape rect;
};

class EngineSet : public GameObject {
public:
    EngineSet() :
        GameObject({0,0},{1,1},0)
    {
        engines.push_back(Engine{});
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir  (0.f,0.f,0.f,0.f);
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const Engine& engine : engines) 
            target.draw(engine);
    }
    std::vector<Engine> engines;
};