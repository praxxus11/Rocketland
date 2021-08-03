#include <SFML/Graphics.hpp>
#include <iostream>

class RocketStats : public GameObject {
public:
    RocketStats(const sf::Vector2f& pos, Rocket* ref) :
        GameObject(pos),
        rocket_ref(ref)
    {
        height.setFont(ResourceManger::getInstance()->getFont(ResourceManger::ResourceTypes::MathFont));
        vel_y.setFont(ResourceManger::getInstance()->getFont(ResourceManger::ResourceTypes::MathFont));
        vel_x.setFont(ResourceManger::getInstance()->getFont(ResourceManger::ResourceTypes::MathFont));
        fuel.setFont(ResourceManger::getInstance()->getFont(ResourceManger::ResourceTypes::MathFont));

        const int font_size = 18;
        height.setFillColor(sf::Color::Black);
        height.setCharacterSize(font_size);
        vel_y.setFillColor(sf::Color::Black);
        vel_y.setCharacterSize(font_size);
        vel_x.setFillColor(sf::Color::Black);
        vel_x.setCharacterSize(font_size);
        fuel.setFillColor(sf::Color(186, 137, 0));
        fuel.setCharacterSize(font_size);

        height.setPosition(sf::Vector2f(0, -3*font_size));
        vel_y.setPosition(sf::Vector2f(0, -2*font_size));
        vel_x.setPosition(sf::Vector2f(0, -font_size));
        fuel.setPosition(sf::Vector2f(0, 0));
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = vel_y.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }

    void update() {
        // labels on top of rocket
        if (Env::show_rocket_stats) {
            height.setString("Height: " + std::to_string(rocket_ref->irlGetPosition().y - 23) + " m");
            vel_y.setString("Velocity-y: " + std::to_string(rocket_ref->getVelocity().y) + " m/s");
            vel_x.setString("Velocity-x: " + std::to_string(rocket_ref->getVelocity().x) + " m/s");
            fuel.setString("Fuel: " + std::to_string(rocket_ref->get_fuel()) + " kg");

            vel_y.setFillColor(abs(rocket_ref->getVelocity().y)<8 ? sf::Color(0, 100, 0) : sf::Color::Red);        
            vel_x.setFillColor(abs(rocket_ref->getVelocity().x)<8 ? sf::Color(0, 100, 0) : sf::Color::Red);        
            const auto& bb = rocket_ref->getGlobalBounds();
            irlSetPosition(sf::Vector2f(bb.left+bb.width/2, bb.top+4));
        }

        if (Env::show_trails) {
            // trailing line
            const float rspeed = sqrt(pow(rocket_ref->getVelocity().x, 2.f) + pow(rocket_ref->getVelocity().y, 2.f));
            const float cred = 255 / (1 + exp(-0.02 * (rspeed - 80)));
            const float cblue = 255 / (1 + exp(0.02 * (rspeed - 80)));
            const sf::Color col = sf::Color(cred, 0, cblue);
            if (!past_pos.size()) {
                sf::VertexArray arr(sf::Lines, 2);
                arr[0].position = rocket_ref->irlGetPosition();
                arr[0].color = col;
                arr[1].position = rocket_ref->irlGetPosition();
                arr[1].color = col;
                past_pos.push_back(arr);
            }
            else {
                if (abs(rocket_ref->irlGetPosition().y - past_pos[past_pos.size() - 1][1].position.y) > 700) {
                    past_pos.clear();
                }
                sf::VertexArray arr(sf::Lines, 2);
                arr[0] = past_pos[past_pos.size()-1][1].position;
                arr[0].color = col;
                arr[1].position = rocket_ref->irlGetPosition();
                arr[1].color = col;
                past_pos.push_back(arr);
            }
        }
    }
private:  
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (Env::show_rocket_stats) {
            states.transform *= getTransform(); 
            target.draw(vel_y, states);
            target.draw(vel_x, states);
            target.draw(height, states);
            target.draw(fuel, states);
        }

        if (Env::show_trails) {
            for (auto line : past_pos) {
                line[0].position = Env::metersToPixels(line[0].position);
                line[1].position = Env::metersToPixels(line[1].position);
                target.draw(line);
            }
        }
    }

    Rocket* rocket_ref; // drawing on top of rocket bounding box
    sf::Text vel_y;
    sf::Text vel_x;
    sf::Text height;
    sf::Text fuel;
    std::vector<sf::VertexArray> past_pos;
};