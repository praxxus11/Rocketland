#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <math.h>
#include <fstream>

#include "Engine.h"
#include "RocketFins.h"

// all position in Rocket should go through
// superclass function in gameobject
// no dealing with direct coordinates in rocket!!!!
class Rocket : public GameObject {
public:
    enum class Status {
        Regular, Explode, BlewUp, Landed
    };
    
    struct stateVectors {
        stateVectors(float ax, float ay, float aa, float fu) :
            accelx(ax),
            accely(ay),
            angular_accel(aa),
            fuel_kg_per_sec(fu)
        {
        }
        float accelx;
        float accely;
        float angular_accel;
        float fuel_kg_per_sec;
        stateVectors operator+(const stateVectors& v) const {
            return stateVectors(
                accelx + v.accelx,
                accely + v.accely,
                angular_accel + v.angular_accel,
                fuel_kg_per_sec + v.fuel_kg_per_sec
            );
        }
    };

    Rocket(const sf::Vector2f& pos, 
    const sf::Vector2f& scal, 
    float rot,
    int pix_tall,
    const sf::Vector2f& velo,
    float ang_vel,
    float ang_accel,
    Gif expls,
    Status stat,
    float mass,
    float f_mass,
    float inertia) : 
        GameObject(pos),
        pixels_tall(pix_tall),
        vel(velo),
        angular_vel(ang_vel),
        explosion_anim(expls),
        status(stat),
        upper_fin(sf::Vector2f(4.5/scal.x, 3/scal.y), this, RocketFins::Type::Upper),
        lower_fin(sf::Vector2f(4.5/scal.x, 34.5/scal.y), this, RocketFins::Type::Lower),
        mass(mass),
        fuel_mass(f_mass),
        angle_inertia(inertia)
    {
        sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketImg));
        explosion_anim.setOrigin(72, 120);
        setScale(scal);
        setRotation(rot);
        
        engines.push_back(Engine(sf::Vector2f(7/getScale().x, 48/getScale().x), this));
        engines.push_back(Engine(sf::Vector2f(4.5/getScale().x, 48/getScale().x), this));
        engines.push_back(Engine(sf::Vector2f(2/getScale().x, 48/getScale().x), this));
    }

    Rocket(const Rocket& r) : 
        GameObject(r.position),
        pixels_tall(r.pixels_tall),
        vel(r.vel),
        angular_vel(r.angular_vel),
        explosion_anim(r.explosion_anim),
        status(r.status),
        upper_fin(r.upper_fin),
        lower_fin(r.lower_fin),
        mass(r.mass),
        fuel_mass(r.fuel_mass),
        angle_inertia(r.angle_inertia),
        engines(r.engines)
    {
        sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketImg));
        explosion_anim.setOrigin(72, 120);    
        setScale(r.getScale());
        setRotation(r.getRotation());

        for (Engine& engine : engines) {
            engine.set_parent(this);
        }
        upper_fin.set_parent(this);
        lower_fin.set_parent(this);
    }
    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = sprite.getLocalBounds();
        ir = getTransform().transformRect(ir);
        sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void update() {
        std::cout << fuel_mass << '\n';
        const float elap = Env::g_elapsed();
        
        switch (status) {
        case Status::Regular: {

            vel.y += Env::gravity*elap;

            // angular_vel += -5./10 * angular_vel * elap;

            for (Engine& engine : engines) {
                engine.update();
            }
            upper_fin.update();
            lower_fin.update();

            stateVectors v(0, 0, 0, 0);
            updateFromEngine(v);
            updateFromWindResistence(v);
            updateFromGravity(v);

            vel.x += v.accelx * elap;
            vel.y += v.accely * elap;
            angular_vel += v.angular_accel * elap;
            fuel_mass = std::max(0.f, fuel_mass + v.fuel_kg_per_sec * elap);
            
            irlSetPosition(sf::Vector2f(position.x + vel.x*elap, position.y + vel.y*elap));
            setRotation(getRotation() + angular_vel*elap);
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
            angular_vel = 0;
            for (Engine& engine : engines) {
                engine.update();
            }
            upper_fin.update();
            lower_fin.update();

            stateVectors v(0, 0, 0, 0);
            updateFromEngine(v);
            if (v.accelx || v.accely || v.angular_accel) {
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
    void setStatus(const Status& s) {
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
            if (fuel_mass) {
                for (const Engine& engine : engines) {
                    target.draw(engine);
                }
            }
            states.transform *= getTransform();
            target.draw(sprite, states);
            target.draw(upper_fin);
            target.draw(lower_fin);

        }
        if (status == Status::Explode) {
            target.draw(explosion_anim);
            // target.draw(*explosion_anim.getBoundingBox().get());
        }
    }
    void updateFromEngine(stateVectors& res) {
        for (const Engine& engine : engines) {
            if (engine.is_engine_on() && fuel_mass) {
                const float engine_force = engine.get_thrust();
                const float rocket_center = 4.5;
                const float engine_displacement_x = getScale().x * engine.irlGetPosition().x - rocket_center;

                // see imgs/referenceImage1
                const float angleA = engine.get_angle();
                const float angleB = 90 - angleA;
                const float angleC = 180/Env::PI * atan2(engine_displacement_x, 25);
                const float angleD = 90 - angleC;
                const float angleE = 180 - angleD - angleB;

                const float line1 = sqrt(25*25 + engine_displacement_x*engine_displacement_x);
                const float line3 = sin(Env::PI/180 * angleE) * engine_force;
                const float torque = line1 * line3;

                const float line2 = cos(Env::PI/180 * angleE) * engine_force;

                // note the getRotation() here: that accounts for the rotation of the rocket itself
                const float lineV = cos(Env::PI/180 * (angleC + getRotation())) * line2; 
                const float lineH = sin(Env::PI/180 * (angleC + getRotation())) * line2;

                res.accely += lineV / get_total_mass();
                res.accelx += lineH / get_total_mass();

                res.angular_accel -= 180/Env::PI * (torque / angle_inertia);


                const float max_flow = 660; // kg per second
                res.fuel_kg_per_sec -= max_flow * engine.get_throttle();
            }
        }
    }
    void updateFromWindResistence(stateVectors& res) {
        res.accely -= (vel.y / 15);
        res.accelx -= (vel.x / 15);
    }
    void updateFromGravity(stateVectors& res) {
        res.accely += Env::gravity;
    }
    float get_total_mass() const {
        return mass + fuel_mass;
    }

    static sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2f vel;
    float angular_vel;
    Gif explosion_anim;
    bool explosion_initialized = 0;
    Status status;
    std::vector<Engine> engines;
    
    float totTime = 0;
    float timeSoFar = 0;

    RocketFins upper_fin;
    RocketFins lower_fin;

    float mass;
    float fuel_mass;
    float angle_inertia;
};