#include <iostream>
#include <vector>

#include "Env.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Gif.h"
#include "Rocket.h"
#include "Floor.h"
#include "CollisionManager.h"

class Manager {
    public:
        Manager() :
            exframes(128, 
                3328,
                26,
                "imgs/explosion_sheet.png")
        {
            int numrocks = 1;
            rockets.reserve(numrocks);
            for (int i=0; i<numrocks; i++) {
                const float fuel_amount = 50000;
                const float rocket_mass = 77000;
                const float inertia = 70000000 + (70000000/(90000 + 77000)) * (fuel_amount + rocket_mass - 90000 + 77000);
                rockets.push_back(Rocket(
                    sf::Vector2f(rand()%10, rand()%30+70),
                    sf::Vector2f(50.f * Env::pixpmeter / 1120, 50.f * Env::pixpmeter / 1120),
                    -0,
                    1120,
                    sf::Vector2f(0, 4),
                    0,
                    0,
                    Gif(3, exframes),
                    Rocket::Status::Regular,
                    rocket_mass, // assume has mass of 7.7e4 kg = around 85 tons
                    fuel_amount, // assume has initial fuel of 9.0e4 kg - around 100 tons 
                    inertia
                ));
                rockets[i].setOrigin(100, 600);
                rockets[i].irlSetPosition(rockets[i].irlGetPosition());
                rockets[i].setScale(rockets[i].getScale());
                rockets[i].setRotation(rockets[i].getRotation());
            }
        }
        ~Manager() 
        {
        }
        void update(sf::RenderWindow& win) {
            drawAll(win);
            for (Rocket& r : rockets) {
                r.setStatus(cm.rocket_floor_collision(r, f));
                r.update(win);
            }
        }
        int get_window_width() const { return Env::ww; }
        int get_window_height() const { return Env::wh; }
    private:
        Frames exframes;
        std::vector<Rocket> rockets;
        Floor f;
        CollisionManager cm;

        void drawAll(sf::RenderWindow& win) const {
            win.draw(f);
            // win.draw(*f.getBoundingBox().get());
            for (const Rocket& r : rockets) {
                // if (r.getStatus() != Rocket::Status::Explode && r.getStatus() != Rocket::Status::BlewUp)
                    // win.draw(*r.getBoundingBox().get());
                win.draw(r);
            }
        }
};