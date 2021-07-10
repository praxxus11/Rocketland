#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

// all position in Rocket should go through
// superclass function in gameobject
// no dealing with direct coordinates in rocket!!!!
class Rocket : public GameObject {
public:
    Rocket() : GameObject({0, 100}, {50.f * Settings::pixpmeter / 1120, 50.f * Settings::pixpmeter / 1120}, 0),
        pixels_tall(1120),
        vel(0, 0),
        accel(0, 0),
        explosion_anim(128, 3328, 26, 3)
    {
        if (!texture.loadFromFile("imgs/ship.png")) {
            std::cout << "Rocket png not loaded";
        }
        texture.setSmooth(true);
        sprite.setTexture(texture);
        explosion_anim.setOrigin(72, 120);
        // std::cout << position.y << ">><<" << position.y << ">>";
    }
    enum class Status {
        Regular, Explode
    };
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Settings::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Settings::pixpmeter, ir.height/Settings::pixpmeter);
    }
    void update(Status status) {
        switch (status) {
        case Status::Regular: {
            float elap = Settings::g_elapsed();

            vel.x += accel.x*elap;
            vel.y += Settings::gravity*elap;
            
            irlSetPosition(sf::Vector2f(position.x + vel.x*elap, position.y + vel.y*elap));
            break;
        }
        case Status::Explode: {
            if (!explosion_initialized) {
                const auto globalbnd = getGlobalBounds();
                explosion_anim.irlSetPosition(sf::Vector2f(globalbnd.left + 0.5*globalbnd.width, globalbnd.top - globalbnd.height));
                constexpr float times_bigger = 1.3; // how large explosion is compared to rocket
                const float scale = (times_bigger * 1120 * getScale().x) / 128;
                explosion_anim.setScale(scale, scale);
                explosion_initialized = 1;
            }
            explosion_anim.update();
            if (explosion_anim.get_curr() <= explosion_anim.get_frames() && 
                !(explosion_anim.get_curr() % (explosion_anim.get_frames() / 10)) &&
                explosion_anim.ison_new_frame()) {
                irlSetPosition(sf::Vector2f(position.x, position.y / 1.5));
            }
            break;
        }
        } // end switch (status)
    } 
    int pixels_tall; // change this code later
    sf::Vector2f vel;
    sf::Vector2f accel;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(sprite, states);
        target.draw(explosion_anim);
    }
    sf::Texture texture;
    sf::Sprite sprite;
    Gif explosion_anim;
    bool explosion_initialized = 0;
};