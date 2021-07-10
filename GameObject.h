#pragma once


#include <SFML/Graphics.hpp>
#include <iostream>

class GameObject: public sf::Drawable, public sf::Transformable {
public:
    virtual sf::FloatRect getGlobalBounds() const = 0;
    void irlSetPosition(sf::Vector2f cor) {
        position = cor;
        setPosition(Settings::convertUnits(cor));
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
protected:
    GameObject(sf::Vector2f pos, sf::Vector2f sc, float rot) :
        position(pos),
        scale(sc),
        rotation(rot) 
    {
    }
    virtual ~GameObject()
    {
    }
    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;
};