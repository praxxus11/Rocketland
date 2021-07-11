#pragma once


#include <SFML/Graphics.hpp>
#include <iostream>

class GameObject: public sf::Drawable, public sf::Transformable {
public:
    virtual sf::FloatRect getGlobalBounds() const = 0;
    void irlSetPosition(sf::Vector2f cor) {
        position = cor;
        setPosition(Settings::metersToPixels(cor));
    }
    sf::Vector2f irlGetPosition() const {
        return position;
    }
    sf::Vector2f getScale() const {
        return scale;
    }
    float getRotation() const {
        return rotation;
    }
    std::unique_ptr<sf::RectangleShape> getBoundingBox() const {
        sf::FloatRect bb = getGlobalBounds();
        std::unique_ptr<sf::RectangleShape> rect(new sf::RectangleShape(Settings::convertSize(sf::Vector2f(bb.width, bb.height))));
        rect->setFillColor(sf::Color::Transparent);
        rect->setOutlineColor(sf::Color::Red);
        rect->setOutlineThickness(3.f);
        sf::Vector2f temp = Settings::metersToPixels(sf::Vector2f(bb.left, bb.top));
        rect->setPosition(temp.x, temp.y);
        return rect;
    }
protected:
    GameObject(sf::Vector2f pos, sf::Vector2f sc, float rot) :
        position(pos),
        scale(sc),
        rotation(rot) 
    {
        setPosition(Settings::metersToPixels(pos));
    }
    virtual ~GameObject()
    {
    }
    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;
};