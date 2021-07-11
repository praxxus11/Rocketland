#include <iostream>

#include "Settings.h"
#include "GameObject.h"
#include "Gif.h"
#include "Rocket.h"
#include "Floor.h"
#include "CollisionManager.h"

class Manager {
    public:
        Manager() 
        {
            r.setOrigin(120, 200);
            r.irlSetPosition(r.irlGetPosition());
            r.setScale(r.getScale());
            r.setRotation(r.getRotation());
        }
        ~Manager() 
        {
        }
        void update(sf::RenderWindow& win) {
            r.setStatus(cm.rocket_floor_collision(r, f));
            r.update();
            drawAll(win);
        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;
        CollisionManager cm;

        void drawAll(sf::RenderWindow& win) const {
            win.draw(f);
            win.draw(*f.getBoundingBox().release());
            if (r.getStatus() != Rocket::Status::Explode && r.getStatus() != Rocket::Status::BlewUp)
                win.draw(*r.getBoundingBox().release());
            win.draw(r);

            // sf::Vertex a(Settings::displ, sf::Color::White);
            // sf::Vertex b(sf::Vector2f(Settings::displ.x, 0), sf::Color::White);
            // sf::Vertex vert[2] {a, b};
            // win.draw(vert, 2, sf::Lines);
        }
};