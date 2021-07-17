#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <math.h>
#include <fstream>
#include <array>

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
    
    struct stateVector {
        stateVector(float ax, float ay, float aa, float fu) :
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
        stateVector operator+(const stateVector& v) const {
            return stateVector(
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
        upper_fin(sf::Vector2f(4.5/scal.x, 3/scal.y), this, RocketFins::Type::Upper, 20),
        lower_fin(sf::Vector2f(4.5/scal.x, 34.5/scal.y), this, RocketFins::Type::Lower, -25),
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
    void update(sf::RenderWindow& win) {
        const float elap = Env::g_elapsed();
        
        switch (status) {
        case Status::Regular: {


            ////////////////////////////
            // totTime += Env::g_elapsed();
            // timeSoFar += Env::g_elapsed();
            // if (timeSoFar > .7) {
            //     std::ofstream fout("python/datas.txt", std::ios_base::app);
            //     timeSoFar = 0;
            //     fout << totTime << " " << position.y << " " << vel.y << " " << vel.x << " " << fuel_mass << '\n';
            // }
            ////////////////////////////


            for (Engine& engine : engines) {
                engine.update();
            }
            upper_fin.update();
            lower_fin.update();

            stateVector v(0, 0, 0, 0);
            updateFromEngine(v);
            // updateFromWindResistence(v);
            updateFromGravity(v);
            updateFromFins(v, win);

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

            stateVector v(0, 0, 0, 0);
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
    void updateFromEngine(stateVector& res) {
        for (const Engine& engine : engines) {
            if (engine.is_engine_on() && fuel_mass) {
                const float engine_force = engine.get_thrust();
                const float rocket_center = 4.5; // horizontal center of the rocket (meters)
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

    void updateFromFins(stateVector& res, sf::RenderWindow& win) {
        
        std::array<RocketFins*, 2> fins = {&upper_fin, &lower_fin};

        for (const RocketFins* fin : fins) {
            const float line1_ref2 = fin->get_radial_dist(); // vert distance from upper fin to middle of rocket
            const float line2_ref2 = line1_ref2 * (Env::PI/180 * angular_vel + 0.000000001);

            const float angleA_ref2 = getRotation();
            const float angleB_ref2 = 90 - angleA_ref2;

            const float lineV_ref2 = -line2_ref2 * cos(Env::PI/180 * angleB_ref2); // negative here, idk why
            const float lineH_ref2 = line2_ref2 * sin(Env::PI/180 * angleB_ref2);

            const float vert_vel_comb = lineV_ref2 + vel.y;
            const float hori_vel_comb = lineH_ref2 + vel.x;


            // imgs/ReferenceImage3
            const float const_mul = (hori_vel_comb*lineH_ref2 + vert_vel_comb*lineV_ref2) / 
                (lineH_ref2*lineH_ref2 + lineV_ref2*lineV_ref2);
            const float r_push_air_h = const_mul * lineH_ref2;
            const float r_push_air_v = const_mul * lineV_ref2;
            
            const float air_push_rock_h = -r_push_air_h;
            const float air_push_rock_v = -r_push_air_v;
            const float air_speed = sqrt(air_push_rock_h*air_push_rock_h + air_push_rock_v*air_push_rock_v);

            const float max_force_from_air = 1e6; // newtons, 1/20 of an engine

            float force_multiplier = 0;
            sf::Vector2f initial_pt(0, 0);
            sf::Vector2f final_pt(500, 0);
        
            initial_pt = (getTransform() * fin->getTransform()).transformPoint(initial_pt);
            final_pt = (getTransform() * fin->getTransform()).transformPoint(final_pt);
        
            sf::Vector2f diff(final_pt-initial_pt);

            // Reference Image 4
            if (diff.x*air_push_rock_h + diff.y*air_push_rock_v < 0) { 
            // differenet direction than movement of rocket, means less force
                force_multiplier = 1 - 0.01*abs(lower_fin.get_angle());
            }
            //Reference Image 5
            else { 
            // same direction as movement, more force
                force_multiplier = 1 - 0.0044*abs(lower_fin.get_angle());
            }
            const float realized_force_from_air = ((air_speed*air_speed) / (90*90)) * max_force_from_air * force_multiplier;

            // 3/4 of the force goes into rotating, and the other 1/4 goes into translation
            // Too lazy to split it because then ill have to either approx or find the moment of intertia
            // which is too slow
            const float translation_force = realized_force_from_air / 4;
            const float air_speed_unit_x = air_push_rock_h / air_speed;
            const float air_speed_unit_y = air_push_rock_v / air_speed;
            const float trans_force_x = translation_force * air_speed_unit_x;
            const float trans_force_y = translation_force * air_speed_unit_y;

            res.accelx += trans_force_x / get_total_mass();
            res.accely += trans_force_y / get_total_mass();

            const float torque = realized_force_from_air * (3.f/4) * abs(fin->get_radial_dist()); // torque = F*r
            
            if (air_push_rock_h*lineV_ref2 + air_push_rock_v*lineH_ref2 < 0)  // torque and rotation opposite direction
                res.angular_accel += (angular_vel > 0 ? -1 : 1) * (torque / angle_inertia);
            else // torque and rotation same direction
                res.angular_accel += (angular_vel > 0 ? 1 : -1) * (torque / angle_inertia);
            

            sf::Vector2f vect(0, 0);
            vect = (getTransform() * fin->getTransform()).transformPoint(vect);
            sf::Vertex line[2] = {
                sf::Vertex(vect, sf::Color::Red),
                sf::Vertex(sf::Vector2f(vect.x+trans_force_x, vect.y+trans_force_y*-1))
            };
            win.draw(line, 2, sf::Lines);        
        }
    }
    void updateFromWindResistence(stateVector& res) {
        res.accely -= (vel.y / 15);
        res.accelx -= (vel.x / 15);
    }
    void updateFromGravity(stateVector& res) {
        res.accely += Env::gravity;
    }
    float get_total_mass() const {
        const float upper_radius = 20; // meters from center of rocket
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