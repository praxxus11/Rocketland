#include <SFML/Graphics.hpp>

class RocketFins : public GameObjectRelative {
public:
    enum class Type {
        Upper, Lower
    };
    // type should be either upperfin or lowerfin
    RocketFins(sf::Vector2f pos, GameObject& parent, Type type) :
        GameObjectRelative(pos, parent),
        angle(0),
        fin_type(type)
    {
        ResourceManger::ResourceTypes load_type = (type == Type::Upper) ? 
            ResourceManger::ResourceTypes::RocketUpperFin : 
            ResourceManger::ResourceTypes::RocketLowerFin;
        fin_sprite.setTexture(ResourceManger::getInstance()->getTexture(load_type));
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = fin_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent.getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        setScale(1,1);
        setScale(angle/90.f * getScale().x, getScale().y);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angle = fin_type == Type::Upper ? 
                std::max(-90.f, angle - 300 * Env::g_elapsed()) :
                std::min(90.f, angle + 300 * Env::g_elapsed());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angle = fin_type == Type::Upper ?
                std::min(90.f, angle + 300 * Env::g_elapsed()) : 
                std::max(-90.f, angle - 300 * Env::g_elapsed());
        }
        else { 
            angle -= 5 * angle * Env::g_elapsed();
        }
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // take note that the order in which the transforms are applied DOES matter
        states.transform *= parent.getTransform() * getTransform(); 
        target.draw(fin_sprite, states);
    }
    sf::Sprite fin_sprite;
    float angle;
    Type fin_type;
};
