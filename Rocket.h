#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <math.h>

#include "Flame.h"

// all position in Rocket should go through
// superclass function in gameobject
// no dealing with direct coordinates in rocket!!!!
class Rocket : public GameObject {
public:
    Rocket() : 
        GameObject({0, 100}, {50.f * Settings::pixpmeter / 1120, 50.f * Settings::pixpmeter / 1120}, 0),
        pixels_tall(1120),
        vel(0, 0),
        accel(0, 0),
        angular_vel(0),
        angular_accel(0),
        explosion_anim(128, 3328, 26, 3),
        status(Status::Regular)
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
        Regular, Explode, BlewUp, Landed
    };
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Settings::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Settings::pixpmeter, ir.height/Settings::pixpmeter);
    }
    void update() {
        std::cout << vel.y << " " << angular_vel << "\n";
        switch (status) {
        case Status::Regular: {
            float elap = Settings::g_elapsed();

            vel.x += accel.x*elap;
            vel.y += Settings::gravity*elap;

            angular_vel += angular_accel*elap;
            rotation += angular_vel;

            angular_vel += -5./10 * angular_vel * elap;

            userInputUpdate(elap);
            
            irlSetPosition(sf::Vector2f(position.x + vel.x*elap, position.y + vel.y*elap));
            setRotation(rotation);
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
            if (explosion_anim.get_curr() < explosion_anim.get_frames() && 
                !(explosion_anim.get_curr() % (explosion_anim.get_frames() / 10)) &&
                explosion_anim.ison_new_frame()) {
                irlSetPosition(sf::Vector2f(position.x, position.y / 1.5));
            }
            if (explosion_anim.get_curr() >= explosion_anim.get_frames()) {
                setStatus(Status::BlewUp);
            }
            break;
        }
        case Status::Landed: {
            vel.x = 0;
            vel.y = 0;
            accel.x = 0;
            accel.y = 0;
            angular_accel = 0;
            angular_vel = 0;
            float elap = Settings::g_elapsed();
            if (userInputUpdate(elap)) {
                sf::Vector2f pos = irlGetPosition();
                pos.y += 0.5; // to not make collisionmanager thing rocket crashed in floor
                irlSetPosition(pos);
                status = Status::Regular;
            }
            break;
        }
        case Status::BlewUp: {
            break;
        }
        } // end switch (status)
    }
    void setStatus(Status s) {
        status = s;
    }
    sf::Vector2f getVelocity() const {
        return vel;
    }
    Status getStatus() const {
        return status;
    }
    int pixels_tall; // change this code later
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (status != Status::Explode && status != Status::BlewUp) {
            states.transform *= getTransform();
            target.draw(sprite, states);
        }
        if (status == Status::Explode) {
            target.draw(explosion_anim);
            target.draw(*explosion_anim.getBoundingBox().release());
        }
    }

    // returns true if update was performed
    bool userInputUpdate(const float& elap) {
        bool updated = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            vel.y += 20 * cos(Settings::PI/180 * rotation) * elap;
            vel.x += 20 * sin(Settings::PI/180 * rotation) * elap;
            updated = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angular_vel += -0.5 * elap;
            updated = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angular_vel += 0.5 * elap;
            updated = true;
        }
        return updated;
    }
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f vel;
    sf::Vector2f accel;
    float angular_vel;
    float angular_accel;
    Gif explosion_anim;
    bool explosion_initialized = 0;
    Status status;
};