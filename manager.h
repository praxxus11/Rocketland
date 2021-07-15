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
                rockets.push_back(Rocket(
                    sf::Vector2f(rand()%80+60, rand()%30+700),
                    sf::Vector2f(50.f * Env::pixpmeter / 1120, 50.f * Env::pixpmeter / 1120),
                    -90,
                    1120,
                    sf::Vector2f(0, rand()%10-100),
                    sf::Vector2f(0, 0),
                    0,
                    0,
                    Gif(3, exframes),
                    Rocket::Status::Regular,
                    77000, // assume has mass of 7.7e4 kg = around 85 tons
                    9000, // assume has initial fuel of 9.0e4 kg - around 100 tons 
                    50000000
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
            for (Rocket& r : rockets) {
                r.setStatus(cm.rocket_floor_collision(r, f));
                r.update();
            }
            drawAll(win);
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