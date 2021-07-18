#include <SFML/Graphics.hpp>
#include <iostream>


class Floor : public GameObject {
public:
    Floor() :
        GameObject({-Env::ww/(2*Env::pixpmeter), 0}) 
    {
        rect.setFillColor(sf::Color(0,100,0));
        rect.setSize(sf::Vector2f(Env::ww, Env::floor_hei));
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = rect.getLocalBounds();
        ir = getTransform().transformRect(ir);
        const sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        irlSetPosition(sf::Vector2f((-Env::ww/2)/Env::pixpmeter + Env::camera_pos.x, 0));
        rect.setSize(sf::Vector2f(Env::ww, Env::floor_hei - (Env::camera_pos.y < 0 ? Env::camera_pos.y * Env::pixpmeter : 0)));
        std::cout << getPosition().x << " " << getPosition().y << '\n';
    }
private:
    sf::RectangleShape rect;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(rect, states);
    }
};