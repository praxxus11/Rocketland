#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>

class CollisionManager {
public:
    CollisionManager() 
    {
    }
    ~CollisionManager()
    {
    }
    // given the position and features of rocket, position of floor,
    // produces status of rocket
    Rocket::Status rocket_floor_collision(const Rocket& r, const  Floor& f) {
        if (r.getStatus() != Rocket::Status::Regular && r.getStatus() != Rocket::Status::Landed) return r.getStatus(); 
        sf::FloatRect rr = r.getGlobalBounds();
        sf::FloatRect fr = f.getGlobalBounds();
        // rr.top - rr.height because getGlobalBound for a GameObject returns
        // bounds in meters. However, game position is in terms of pixels, where
        // going down is positive direction. Need to compensate for this transform meter to pixesl
        if (sf::FloatRect(rr.left, rr.top-rr.height, rr.width, rr.top)
            .intersects(sf::FloatRect(fr.left, fr.top-fr.height, fr.width, fr.height)) || 
            rr.top - rr.height < fr.top) {
            if ((r.getRotation() < 10 || r.getRotation() > 350) && abs(r.getVelocity().y) < 20 && abs(r.getVelocity().x) < 20) {
                return Rocket::Status::Landed;
            } 
            return Rocket::Status::Explode;
        }
        return Rocket::Status::Regular;
    }
private:
};