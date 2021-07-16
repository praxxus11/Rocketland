#include <SFML/Graphics.hpp>
#include <iostream>


class Engine : public GameObjectRelative {
public:
    Engine(sf::Vector2f pos, GameObject& parent) :
        GameObjectRelative(pos, parent),
        angular_delta(0),
        throttle(0.5f),
        max_thrust(2.3e6)
    {
        flame_sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketFlame));
        flame_sprite.setOrigin(522/2, 0);
    }
    Engine(const Engine& e) :
        GameObjectRelative(e.position, e.parent),
        angular_delta(e.angular_delta),
        throttle(e.throttle),
        max_thrust(e.max_thrust)
    {
        flame_sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketFlame));
        flame_sprite.setOrigin(522/2, 0);
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
        float scx = 1;
        float scy = (rand()%1000)/2000. + 1;
        setScale(scx, scy * (0.4 + throttle));
        setScale(0.3*getScale().x, 0.5*getScale().y);
        setRotation(angular_delta);
        engine_on = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angular_delta = std::max(-25.f, angular_delta-100*Env::g_elapsed());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angular_delta = std::min(25.f, angular_delta+100*Env::g_elapsed());
        }
        else { 
            angular_delta -= 5*angular_delta*Env::g_elapsed();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            throttle = std::min(1.f, throttle + 1.f*Env::g_elapsed());
        }   
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            throttle = std::max(0.05f, throttle - 1.f*Env::g_elapsed());
        }
    }
    float get_angle() const {
        return angular_delta;
    }
    float is_engine_on() const {
        return engine_on;
    }
    float get_thrust() const {
        return max_thrust * throttle;
    }
    float get_throttle() const {
        return throttle;
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // take note that the order in which the transforms are applied DOES matter
        if (is_engine_on()) {
            states.transform *= parent.getTransform() * getTransform(); 
            target.draw(flame_sprite, states);
        }
    }
    sf::Sprite flame_sprite;
    float angular_delta; // -45 -> 45 degress, where 0 degrees is downward
    float throttle; // 0.05 -> 1.0
    bool engine_on;
    float max_thrust;
};
