#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <math.h>

#include "Engine.h"

// all position in Rocket should go through
// superclass function in gameobject
// no dealing with direct coordinates in rocket!!!!
class Rocket : public GameObject {
public:
    enum class Status {
        Regular, Explode, BlewUp, Landed
    };
    Rocket(const sf::Vector2f& pos, 
    const sf::Vector2f& scal, 
    float rot,
    int pix_tall,
    const sf::Vector2f& velo,
    const sf::Vector2f& acce,
    float ang_vel,
    float ang_accel,
    Gif expls,
    Status stat) : 
        GameObject(pos),
        pixels_tall(pix_tall),
        vel(velo),
        accel(acce),
        angular_vel(ang_vel),
        angular_accel(ang_accel),
        explosion_anim(expls),
        status(stat),
        engine({0, 0}, *this)
    {
        sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketImg));
        explosion_anim.setOrigin(72, 120);
        setScale(scal);
        setRotation(rot);
    }

    Rocket(const Rocket& r) : 
        GameObject(r.position),
        pixels_tall(r.pixels_tall),
        vel(r.vel),
        accel(r.accel),
        angular_vel(r.angular_vel),
        angular_accel(r.angular_accel),
        explosion_anim(r.explosion_anim),
        status(r.status),
        engine({0, 0}, *this)
    {
        // if (!texture.loadFromFile("imgs/ship.png")) {
        //     std::cout << "Rocket png not loaded";
        // }
        // texture.setSmooth(true);
        sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketImg));
        explosion_anim.setOrigin(72, 120);    
        setScale(r.getScale());
        setRotation(r.getRotation());
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        // std::cout << vel.y << " " << angular_vel << "\n";
        switch (status) {
        case Status::Regular: {
            const float elap = Env::g_elapsed();

            vel.x += accel.x*elap;
            vel.y += Env::gravity*elap;

            angular_vel += angular_accel*elap;
            setRotation(getRotation() + angular_vel*elap);

            angular_vel += -5./10 * angular_vel * elap;

            userInputUpdate(elap);
            
            irlSetPosition(sf::Vector2f(position.x + vel.x*elap, position.y + vel.y*elap));
            setRotation(getRotation());

            engine.update(getScale().x);
            break;
        }
        case Status::Explode: {
            if (!explosion_initialized) {
                const auto globalbnd = getGlobalBounds();
                explosion_anim.irlSetPosition(sf::Vector2f(globalbnd.left + 0.5*globalbnd.width, globalbnd.top - globalbnd.height));
                float times_bigger = 0.5 + std::min(sqrt(vel.x*vel.x + vel.y*vel.y) / 30, 2.f); // how large explosion is compared to rocket
                const float scale = (times_bigger * 1120 * getScale().x) / 128;
                explosion_anim.setScale(rand()%2 ? scale : -scale, scale); // randomly flip the explosion animation
                explosion_initialized = 1;
            }
            explosion_anim.update();
            if (explosion_anim.get_curr() < explosion_anim.number_frames() && 
                !(explosion_anim.get_curr() % (explosion_anim.number_frames() / 10)) &&
                explosion_anim.ison_new_frame()) {
                irlSetPosition(sf::Vector2f(position.x, position.y / 1.5));
            }
            else if (explosion_anim.get_curr() >= explosion_anim.number_frames()) {
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
            float elap = Env::g_elapsed();
            if (userInputUpdate(elap)) {
                sf::Vector2f pos = irlGetPosition();
                pos.y += 0.1; // to not make collisionmanager thing rocket crashed in floor
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
        if (status == Status::Regular || status == Status::Landed) {
            states.transform *= getTransform();
            target.draw(sprite, states);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) target.draw(engine, states);
        }
        if (status == Status::Explode) {
            target.draw(explosion_anim);
            // target.draw(*explosion_anim.getBoundingBox().get());
        }
    }

    // returns true if update was performed
    bool userInputUpdate(const float& elap) {
        bool updated = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            vel.y += 20 * cos(Env::PI/180 * getRotation()) * elap;
            vel.x += 20 * sin(Env::PI/180 * getRotation()) * elap;
            updated = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            angular_vel += -10 * elap;
            updated = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            angular_vel += 10 * elap;
            updated = true;
        }
        return updated;
    }
    static sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f vel;
    sf::Vector2f accel;
    float angular_vel;
    float angular_accel;
    Gif explosion_anim;
    bool explosion_initialized = 0;
    Status status;
    Engine engine;
};