#include <SFML/Graphics.hpp>
#include <iostream>
#include <assert.h>


class Engine : public GameObjectRelative {
public:
    Engine(sf::Vector2f pos, GameObject* parent) :
        GameObjectRelative(pos, parent),
        angle(0),
        throttle(0.5f),
        angle_vel(0),
        throttle_vel(0),
        max_thrust(2.3e6)
    {
#if defined(CPU)
        flame_sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketFlame));
#endif
        
        flame_sprite.setOrigin(522/2, 0);
    }
    Engine(const Engine& e) :
        GameObjectRelative(e.position, e.parent),
        angle(e.angle),
        throttle(e.throttle),
        max_thrust(e.max_thrust)
    {
#if defined(CPU)
        flame_sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketFlame));
#endif
    
        flame_sprite.setOrigin(522/2, 0);
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = flame_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent->getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        const float scale = parent->getScale().x;
        float scx = 1;
        float scy = (rand()%1000)/2000. + 1;
        setScale(scx, scy * (0.4 + throttle));
        setScale(0.25*getScale().x, 0.5*getScale().y);
        setRotation(angle);

        if (angle_vel < 0) angle = std::max(-15.f, angle + angle_vel * Env::g_elapsed());
        else angle = std::min(15.f, angle + angle_vel * Env::g_elapsed());

        if (throttle_vel < 0) throttle = std::max(0.f, throttle + throttle_vel * Env::g_elapsed());
        else throttle = std::min(1.f, throttle + throttle_vel * Env::g_elapsed());
    }
    float get_angle() const {
        return angle;
    }
    float is_engine_on() const {
        return (throttle >= 0.4);
    }
    float get_thrust() const {
        return max_thrust * throttle;
    }
    float get_throttle() const {
        return throttle;
    }
    void set_throttle_vel(float tv) {
        throttle_vel = tv;
    }
    void set_angle_vel(float av) {
        angle_vel = av;
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // take note that the order in which the transforms are applied DOES matter
        if (is_engine_on()) {
            states.transform *= parent->getTransform() * getTransform(); 
            target.draw(flame_sprite, states);
        }
    }
#if defined(CPU)
    sf::Sprite flame_sprite;
#elif defined(GPU)
    sf::RectangleShape flame_sprite = sf::RectangleShape(sf::Vector2f(522, 1783));
#endif

    float angle; // -45 -> 45 degress, where 0 degrees is downward
    float throttle; // 0.05 -> 1.0
    float angle_vel;
    float throttle_vel;

    float max_thrust;
};
