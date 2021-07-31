#include <SFML/Graphics.hpp>
#include <iostream>

class Cloud : public GameObject {
public:
    Cloud(const sf::Vector2f& pos, ResourceManger::ResourceTypes type) :
        GameObject(pos)
    {
#if defined(CPU)
        cloud_sprite.setTexture(ResourceManger::getInstance()->getTexture(type));
#endif
    }

    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = cloud_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    
private:
#if defined(CPU)
    sf::Sprite cloud_sprite;
#elif defined(GPU)
    sf::RectangleShape cloud_sprite;
#endif
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform(); 
        target.draw(cloud_sprite, states);
    }
};