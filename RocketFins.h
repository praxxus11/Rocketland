#include <SFML/Graphics.hpp>

class RocketFins : public GameObjectRelative {
public:
    enum class Type {
        Upper, Lower
    };
    // type should be either upperfin or lowerfin
    RocketFins(sf::Vector2f pos, GameObject* parent, Type type, float rad_dist) :
        GameObjectRelative(pos, parent),
        angle(0),
        angular_vel(0),
        fin_type(type),
        radial_distance(rad_dist)
    {
#if defined(CPU)
        ResourceManger::ResourceTypes load_type = (type == Type::Upper) ? 
            ResourceManger::ResourceTypes::RocketUpperFin : 
            ResourceManger::ResourceTypes::RocketLowerFin;
        fin_sprite.setTexture(ResourceManger::getInstance()->getTexture(load_type));
#endif
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = fin_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent->getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        setScale(1,1);
        setScale(angle/90.f * getScale().x, getScale().y);


        if (angular_vel < 0) angle = std::max(-90.f, angle + angular_vel * Env::g_elapsed());
        else angle = std::min(90.f, angle + angular_vel * Env::g_elapsed());
    }
    float get_angle() const {
        return angle;
    }
    void set_angular_vel(float angv) {
        angular_vel = angv;
    }
    float get_radial_dist() const {
        return radial_distance;
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        // take note that the order in which the transforms are applied DOES matter
        states.transform *= parent->getTransform() * getTransform(); 
        target.draw(fin_sprite, states);
    }
#if defined(CPU)
    sf::Sprite fin_sprite;
#elif defined(GPU)
    sf::RectangleShape fin_sprite;
#endif

    float angle;
    float angular_vel;
    Type fin_type;
    float radial_distance;
};
