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
    Rocket::Status rocket_floor_collision(const Rocket& r, const  Floor& f) {
        sf::FloatRect rr = r.getGlobalBounds();
        sf::FloatRect fr = f.getGlobalBounds();
        // rr.top - rr.height because getGlobalBound for a GameObject returns
        // bounds in meters. However, game position is in terms of pixels, where
        // going down is positive direction. Need to compensate for this transform meter to pixesl
        if (sf::FloatRect(rr.left, rr.top-rr.height, rr.width, rr.top)
            .intersects(sf::FloatRect(fr.left, fr.top-fr.height, fr.width, fr.height))
            || rr.top - rr.height < fr.top) {
            if (abs(r.getRotation()) < 5 && r.getVelocity().y > -6) {
                return Rocket::Status::Landed;
            } 
            return Rocket::Status::Explode;
        }
        return Rocket::Status::Regular;
    }
private:
};