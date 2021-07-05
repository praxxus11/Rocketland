#include <iostream>

#include "Settings.h"
#include "Rocket.h"
#include "Floor.h"
#include "CollisionManager.h"
#include "GIFhandler.h"

class Manager {
    public:
        Manager() : 
            gh(128, 3328, 26, 0.4)
        {
            gh.setScale(5,5);
        }
        void update(sf::RenderWindow& win) {
            float elap = Settings::g_elapsed();

            if (!cm.rocket_floor_collision(r, f)) {
                r.vel.x += r.accel.x*elap;
                r.vel.y += Settings::gravity*elap;
                
                sf::Vector2f newPos {Settings::convertUnits(sf::Vector2f(r.position.x, r.position.y))};
                r.setPosition(newPos.x, newPos.y);

                r.position.x += r.vel.x*elap;
                r.position.y += r.vel.y*elap;
            }
            r.setScale(r.scale.x, r.scale.y);
            r.setRotation(r.rotation);
            drawAll(win);
            gh.update();

        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;
        CollisionManager cm;
        GIFhandler gh;

        void drawAll(sf::RenderWindow& win) {
            sf::FloatRect bb = r.getGlobalBounds();
            sf::RectangleShape rect(sf::Vector2f(bb.width, bb.height));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Red);
            rect.setOutlineThickness(3.f);
            rect.setPosition(bb.left, bb.top);
            win.draw(r);
            win.draw(rect);
            win.draw(f);
            sf::Vertex a(Settings::displ, sf::Color::White);
            sf::Vertex b(sf::Vector2f(Settings::displ.x, 0), sf::Color::White);
            sf::Vertex vert[2] {a, b};
            win.draw(vert, 2, sf::Lines);
            win.draw(gh);
        }
};