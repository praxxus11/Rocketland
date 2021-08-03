#include <iostream>
#include <vector>

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#include "Env.h"
#include "ResourceManager.h"
#include "GameObject.h"
#include "Gif.h"
#include "Rocket.h"
#include "Floor.h"
#include "CollisionManager.h"
#include "AIManager.h"
#include "DecorationsManager.h"
#include "NNDrawer.h"

class Manager {
    public:
        Manager() 
        :
            exframes(128, 3328, 26, "imgs/explosion_sheet.png"),
            nndr(sf::Vector2f(50, 50), ai_manager.get_layer_sizes())
        {
            rockets.reserve(Env::num_rocks);
            for (int i=0; i<Env::num_rocks; i++) {
                const float fuel_amount = 50000;
                const float rocket_mass = 77000;
                const float inertia = 70000000 + (70000000/(90000 + 77000)) * (fuel_amount + rocket_mass - 90000 + 77000);
                rockets.emplace_back(
                    sf::Vector2f(rand()%10+40, rand()%10 + 1000),
                    sf::Vector2f(50.f * Env::pixpmeter / 1120, 50.f * Env::pixpmeter / 1120),
                    -90,
                    1120,
                    sf::Vector2f(0, -80),
                    0,
                    0,
                    Gif(3, exframes),
                    Rocket::Status::Regular,
                    rocket_mass, // assume has mass of 7.7e4 kg = around 85 tons
                    fuel_amount, // assume has initial fuel of 9.0e4 kg - around 100 tons 
                    inertia
                );
                rockets[i].setOrigin(100, 600);
                rockets[i].irlSetPosition(rockets[i].irlGetPosition());
                rockets[i].setScale(rockets[i].getScale());
                rockets[i].setRotation(rockets[i].getRotation());
            }
            dm.init_rocket_labels(rockets);
            // ai_manager.init_random(rockets);
            ai_manager.init_from_file(rockets, "C:\\Users\\Eric\\ProgrammingProjectsCpp\\RocketSaves\\V2Run1\\iteration3300.txt");
        }
        ~Manager() 
        {
        }

        void update() {
            for (Rocket& r : rockets) {
                r.setStatus(cm.rocket_floor_collision(r, f));
                r.update();
            }

            f.update(); // make sure to update rocket before floor
            ai_manager.update_rockets();
            dm.update();
            nndr.update(ai_manager.get_wb(), ai_manager.get_lbl_activations(), rockets[0]);
        }

        void draw(sf::RenderWindow& win) const {
            win.draw(f);
            dm.draw(win);
            // win.draw(*f.getBoundingBox().get());
            for (const Rocket& r : rockets) {
                // if (r.getStatus() != Rocket::Status::Explode && r.getStatus() != Rocket::Status::BlewUp)
                    // win.draw(*r.getBoundingBox().get());
                win.draw(r);
            }
            win.draw(nndr);
        }
        int get_window_width() const { return Env::ww; }
        int get_window_height() const { return Env::wh; }
        sf::Vector2f get_rocket_pos() const {
            return rockets[0].irlGetPosition();
        }
    private:
        Frames exframes;
        std::vector<Rocket> rockets;
        Floor f;
        CollisionManager cm;
        AIManager ai_manager;
        DecorationsManager dm;
        NNDrawer nndr;
};