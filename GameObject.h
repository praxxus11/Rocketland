#pragma once


#include <SFML/Graphics.hpp>
#include <iostream>

class GameObject: public sf::Drawable, public sf::Transformable {
public:
    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;
    virtual sf::FloatRect getGlobalBounds() const = 0;
protected:
    GameObject(sf::Vector2f pos, sf::Vector2f sc, float rot) :
        position(pos),
        scale(sc),
        rotation(rot) 
    {
    }
    ~GameObject()
    {
    }
};