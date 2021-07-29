#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>
#include <math.h>
#include <fstream>
#include <array>

#include "Engine.h"
#include "RocketFins.h"

struct StateParams {
    StateParams(
        float a, float b,
        float c, float d,
        float e, float f, 
        float g, float h, 
        float i, float j, 
        float k, float l,
        float m, float n) :
        posy(a), posx(b),
        vely(c), velx(d),
        angle(e), angle_vel(f),
        e1_thr(g), e1_angle(h),
        e2_thr(i), e2_angle(j),
        e3_thr(k), e3_angle(l),
        uflp_angle(m), lflp_angle(n)
    {
    }
    float posy, posx;
    float vely, velx;
    float angle, angle_vel;
    float e1_thr, e1_angle;
    float e2_thr, e2_angle;
    float e3_thr, e3_angle;
    float uflp_angle, lflp_angle;
};

struct ControlParams {
    ControlParams(
        float e1t, float e1a,
        float e2t, float e2a, 
        float e3t, float e3a, 
        float uflp, float lflp) :
        e1_thr_vel(e1t), e1_angle_vel(e1a),
        e2_thr_vel(e2t), e2_angle_vel(e2a),
        e3_thr_vel(e3t), e3_angle_vel(e3a),
        uflp_angle_vel(uflp), lflp_angle_vel(lflp)
    {
    }
    float e1_thr_vel, e1_angle_vel;
    float e2_thr_vel, e2_angle_vel;
    float e3_thr_vel, e3_angle_vel;
    float uflp_angle_vel, lflp_angle_vel;
};


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
    Status stat,
    float mass,
    float f_mass,
    float inertia) : 
        GameObject(pos),
        pixels_tall(pix_tall),
        vel(velo),
        angular_vel(ang_vel),
        status(stat),
        upper_fin(sf::Vector2f(4.5/scal.x, 3/scal.y), this, RocketFins::Type::Upper, 20),
        lower_fin(sf::Vector2f(4.5/scal.x, 34.5/scal.y), this, RocketFins::Type::Lower, -25),
        mass(mass),
        fuel_mass(f_mass),
        angle_inertia(inertia)
    {
#if defined(CPU)
        sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketImg));
#endif
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
        status(r.status),
        upper_fin(r.upper_fin),
        lower_fin(r.lower_fin),
        mass(r.mass),
        fuel_mass(r.fuel_mass),
        angle_inertia(r.angle_inertia),
        engines(r.engines)
    {
#if defined(CPU)
        sprite.setTexture(ResourceManger::getInstance()->getTexture(ResourceManger::ResourceTypes::RocketImg));
#endif  
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
        const float elap = Env::g_elapsed();
        
        switch (status) {
        case Status::Regular: {


            ////////////////////////////
            // totTime += Env::g_elapsed();
            // timeSoFar += Env::g_elapsed();
            // if (timeSoFar > 8) {
            //     std::ofstream fout("python/datas.txt", std::ios_base::app);
            //     timeSoFar = 0;
            //     fout << totTime << " " << position.y << " " << vel.y << " " << vel.x << " " << getRotation() << '\n';
            // }
            ////////////////////////////
            irlSetPosition(sf::Vector2f(position.x + vel.x*elap, position.y + vel.y*elap));
            setRotation(getRotation() + angular_vel*elap);
            setScale(sf::Vector2f(50.f * Env::pixpmeter / 1120, 50.f * Env::pixpmeter / 1120));

            for (Engine& engine : engines) {
                engine.update();
            }

            engines[0].irlSetDisplacement(sf::Vector2f(7/getScale().x, 48/getScale().x));
            engines[1].irlSetDisplacement(sf::Vector2f(4.5/getScale().x, 48/getScale().x));
            engines[2].irlSetDisplacement(sf::Vector2f(2/getScale().x, 48/getScale().x));

            upper_fin.update();
            lower_fin.update();

            stateVector v(0, 0, 0, 0);
            updateFromEngine(v);
            // std::cout << "Engine" << v.angular_accel << '\n';
            updateFromWindResistence(v);
            // std::cout << "Wind" << v.angular_accel << '\n';
            updateFromGravity(v);
            // std::cout << "Gravity" << v.angular_accel << '\n';
            updateFromFins(v);
            // std::cout << "Finds" << v.angular_accel << "\n\n";

            vel.x += v.accelx * elap;
            vel.y += v.accely * elap;
            angular_vel += v.angular_accel * elap;
            fuel_mass = std::max(0.f, fuel_mass + v.fuel_kg_per_sec * elap);
            break;
        }
        case Status::Explode: {
            setStatus(Status::BlewUp);
            break;
        }
        case Status::Landed: {
            vel.x = 0;
            vel.y = 0;
            angular_vel = 0;
            irlSetPosition(sf::Vector2f(position.x, position.y));
            setScale(sf::Vector2f(50.f * Env::pixpmeter / 1120, 50.f * Env::pixpmeter / 1120));
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
    float get_angular_vel() const {
        return angular_vel;
    }
    int pixels_tall; // change this code later

    StateParams get_rocket_params() const {
        return StateParams(
            position.y, position.x,
            vel.y, vel.x,
            getRotation(), angular_vel,
            engines[0].get_throttle(), engines[0].get_angle(),
            engines[1].get_throttle(), engines[1].get_angle(),
            engines[2].get_throttle(), engines[2].get_angle(),
            upper_fin.get_angle(), lower_fin.get_angle()
        );
    }

    void update_params(const ControlParams& contr) {
        engines[0].set_throttle_vel(contr.e1_thr_vel);
        engines[0].set_angle_vel(contr.e1_angle_vel * 100);

        engines[1].set_throttle_vel(contr.e2_thr_vel);
        engines[1].set_angle_vel(contr.e2_angle_vel * 100);

        engines[2].set_throttle_vel(contr.e3_thr_vel);
        engines[2].set_angle_vel(contr.e3_angle_vel * 100);

        upper_fin.set_angular_vel(contr.uflp_angle_vel * 300);
        lower_fin.set_angular_vel(contr.lflp_angle_vel * 300);
    }
    
    void reset_rocket() {
        irlSetPosition(sf::Vector2f(rand()%10-5, rand()%400 + 1100));
        vel.x = rand()%20-10; vel.y = rand()%5-130;
        setRotation((rand()%2 ? 1 : -1) * (rand()%20-100));
        
        angular_vel = rand()%30-15;
        status = Status::Regular;
        fuel_mass = rand()%100 + 30000;
    }

    float is_engine_running(int i) const {
        return fuel_mass && engines[i].is_engine_on();
    }
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

    void updateFromFins(stateVector& res) {

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

            const float max_force_from_air = 1e6; // newtons, 1/2 of an engine

            float force_multiplier = 1;
            sf::Vector2f initial_pt(0, 0);
            sf::Vector2f final_pt(1, 0);
        
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
            
            if (air_push_rock_h*lineH_ref2 + air_push_rock_v*lineV_ref2 < 0)  // torque and rotation opposite direction
                res.angular_accel += (angular_vel > 0 ? -1 : 1) * (torque / angle_inertia);
            else // torque and rotation same direction
                res.angular_accel += (angular_vel > 0 ? 1 : -1) * (torque / angle_inertia);
        }
    }
    void updateFromWindResistence(stateVector& res) {
        res.accely -= (vel.y / 60);
        res.accelx -= (vel.x / 60);
    }
    void updateFromGravity(stateVector& res) {
        res.accely += Env::gravity;
    }
    float get_total_mass() const {
        return mass + fuel_mass;
    }
#if defined(CPU)
    sf::Sprite sprite;
#elif defined(GPU)
    sf::RectangleShape sprite = sf::RectangleShape(sf::Vector2f(210, 1120));   
#endif 
    sf::Vector2f vel;
    float angular_vel;
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