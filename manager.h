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
            r.accel.y = -1 * Settings::gravity;

            r.vel.x += r.accel.x*elap;
            r.vel.y += r.accel.y*elap;

            r.position.x += r.vel.x*elap;
            r.position.y += r.vel.y*elap;
            r.setScale(r.scale.x, r.scale.y);
            r.setRotation(r.rotation);
            r.setPosition(r.position.x*Settings::pixpmeter, r.position.y*Settings::pixpmeter);
            drawAll(win);
        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;

        void drawAll(sf::RenderWindow& win) {
            win.draw(r);
            win.draw(f);
            sf::Vertex a(Settings::displ, sf::Color::White);
            sf::Vertex b(sf::Vector2f(Settings::displ.x, 0), sf::Color::White);
            sf::Vertex vert[2] {a, b};
            win.draw(vert, 2, sf::Lines);
        }
};