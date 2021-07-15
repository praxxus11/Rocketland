#include <SFML/Graphics.hpp>
#include <iostream>


class Engine : public GameObjectRelative {
public:
    Engine(sf::Vector2f pos, GameObject& parent) :
        GameObjectRelative(pos, parent),
        angular_delta(0),
        throttle(1.f) 
    {
        flame_sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketFlame));
        flame_sprite.setOrigin(522/2, 0);
    }
    Engine(const Engine& e) :
        GameObjectRelative(e.position, e.parent)
    {
        flame_sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketFlame));

    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = flame_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent.getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        const float scale = parent.getScale().x;
        irlSetDisplacement(sf::Vector2f(4.5/scale, 48/scale));
        float scx = 1;
        float scy = (rand()%1000)/2000. + 1;
        setScale(scx, scy * (0.4 + get_throttle()));
        setScale(0.3*getScale().x, 0.5*getScale().y);
        setRotation(angular_delta);
        engine_on = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angular_delta = std::max(-45.f, angular_delta-100*Env::g_elapsed());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angular_delta = std::min(45.f, angular_delta+100*Env::g_elapsed());
        }
        else { 
            angular_delta -= 5*angular_delta*Env::g_elapsed();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            throttle = std::min(1.f, throttle + 1.f*Env::g_elapsed());
        }   
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            throttle = std::max(0.4f, throttle - 1.f*Env::g_elapsed());
        }
    }
    float get_angle() const {
        return angular_delta;
    }
    float is_engine_on() const {
        return engine_on;
    }
    float get_throttle() const {
        return throttle;
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // take note that the order in which the transforms are applied DOES matter
        states.transform *= parent.getTransform() * getTransform(); 
        if (is_engine_on())
            target.draw(flame_sprite, states);
    }
    sf::Sprite flame_sprite;
    float angular_delta; // -45 -> 45 degress, where 0 degrees is downward
    float throttle; // 0.4 -> 1.0
    bool engine_on;
};
