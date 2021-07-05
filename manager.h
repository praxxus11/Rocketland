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

            r.vel.x += r.accel.x*elap;
            r.vel.y += Settings::gravity*elap;

            r.setScale(r.scale.x, r.scale.y);
            r.position.x += r.vel.x*elap;
            r.position.y += r.vel.y*elap;
            r.setRotation(r.rotation);
            sf::Vector2f newPos {Settings::convertUnits(sf::Vector2f(r.position.x, r.position.y))};
            r.setPosition(newPos.x, newPos.y);
            drawAll(win);

            if (cm.rocket_floor(r, f)) {
                r.vel.y = 0;
                r.position.y = 300;
            }
        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;
        CollisionManager cm;

        void drawAll(sf::RenderWindow& win) {
            win.draw(r);
            win.draw(f);
            sf::Vertex a(Settings::displ, sf::Color::White);
            sf::Vertex b(sf::Vector2f(Settings::displ.x, 0), sf::Color::White);
            sf::Vertex vert[2] {a, b};
            win.draw(vert, 2, sf::Lines);
        }
};