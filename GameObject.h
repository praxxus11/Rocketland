#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>

class GameObject: public sf::Drawable, public sf::Transformable {
public:
    virtual sf::FloatRect getGlobalBounds() const = 0;

    void irlSetPosition(sf::Vector2f cor) {
        position = cor;
        setPosition(Env::metersToPixels(cor));
    }

    const sf::Vector2f& irlGetPosition() const {
        return position;
    }

    std::unique_ptr<sf::RectangleShape> getBoundingBox() const {
        const sf::FloatRect bb = getGlobalBounds();
        std::unique_ptr<sf::RectangleShape> rect(
            new sf::RectangleShape(Env::convertSize(sf::Vector2f(bb.width, bb.height))));
        rect->setFillColor(sf::Color::Transparent);
        rect->setOutlineColor(sf::Color::Red);
        rect->setOutlineThickness(3.f);
        sf::Vector2f temp = Env::metersToPixels(sf::Vector2f(bb.left, bb.top));
        rect->setPosition(temp.x, temp.y);
        return rect;
    }

protected:
    GameObject(sf::Vector2f pos) :
        position(pos)
    {
        setPosition(Env::metersToPixels(pos));
    }
    virtual ~GameObject()
    {
    }
    sf::Vector2f position;
};

class GameObjectRelative : public GameObject {
public:
    void irlSetDisplacement(sf::Vector2f cor) {
        position = cor;
        setPosition(Env::convertSize(cor));
    }
    GameObjectRelative(sf::Vector2f pos, GameObject& par) :
        GameObject(pos),
        parent(par)
    {
        irlSetDisplacement(pos);
    }
protected:
    // the gameobject that this one is relative to
    GameObject& parent;
};