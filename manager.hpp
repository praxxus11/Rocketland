#include <iostream>

#include "Settings.h"
#include "Rocket.h"
#include "Floor.h"
#include "CollisionManager.h"

class Manager {
    public:
        Manager() 
        {
        }
        void update(sf::RenderWindow& win) {
            float elap = Settings::g_elapsed();
            r.accel.second = -1 * Settings::gravity;

            r.vel.first += r.accel.first*elap;
            r.vel.second += r.accel.second*elap;

            r.position.first += r.vel.first*elap;
            r.position.second += r.vel.second*elap;
            r.setScale(r.scale.first, r.scale.second);
            r.setRotation(r.rotation);
            r.setPosition(r.position.first*Settings::pixpmeter, r.position.second*Settings::pixpmeter);
            win.draw(r);
            win.draw(f);

            // if ((50+r.position.second)*Settings::pixpmeter > Settings::wh - Settings::floor_hei)
            //     std::cout << Settings::time_so_far << std::endl;
            // else
            //     Settings::time_so_far += elap;
        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;
};