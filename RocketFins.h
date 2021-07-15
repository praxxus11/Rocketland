#include <SFML/Graphics.hpp>

class RocketFins : public GameObjectRelative {
public:
    // type should be either upperfin or lowerfin
    RocketFins(sf::Vector2f pos, GameObject& parent, ResourceManger::ResourceTypes type) :
        GameObjectRelative(pos, parent),
        angle(0)
    {
        fin_sprite.setTexture(ResourceManger::getInstance()->getTexture(type));
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = fin_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent.getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        const float parent_scale = parent.getScale().x;
        irlSetDisplacement(sf::Vector2f(4.5/parent_scale, 3/parent_scale));
        float scale = 0.35;
        setScale(scale/parent.getScale().x, scale/parent.getScale().y);

        // if (angle <= 0) {
        //     setScale(std::min(-0.2f, angle/90.f), 1);
        // }
        // else {
        //     setScale(std::max(0.2f, angle/90.f), 1);
        // }
        setScale(angle/90.f, 1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angle = std::max(-90.f, angle-300*Env::g_elapsed());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angle = std::min(90.f, angle+300*Env::g_elapsed());
        }
        else { 
            angle -= 5*angle*Env::g_elapsed();
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
};

class RocketFins2 : public GameObjectRelative {
public:
    // type should be either upperfin or lowerfin
    RocketFins2(sf::Vector2f pos, GameObject& parent, ResourceManger::ResourceTypes type) :
        GameObjectRelative(pos, parent),
        angle(0)
    {
        fin_sprite.setTexture(ResourceManger::getInstance()->getTexture(type));
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = fin_sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        ir = parent.getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        const float parent_scale = parent.getScale().x;
        irlSetDisplacement(sf::Vector2f(4.5/parent_scale, 34.6/parent_scale));
        float scale = 0.5;
        setScale(scale/parent.getScale().x, scale/parent.getScale().y);

        // if (angle <= 0) {
        //     setScale(std::min(-0.2f, angle/90.f), 1);
        // }
        // else {
        //     setScale(std::max(0.2f, angle/90.f), 1);
        // }
        setScale(angle/90.f, 1);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angle = std::max(-90.f, angle-300*Env::g_elapsed());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angle = std::min(90.f, angle+300*Env::g_elapsed());
        }
        else { 
            angle -= 5*angle*Env::g_elapsed();
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
};