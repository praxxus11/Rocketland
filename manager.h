#include <iostream>

#include "Env.h"
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
                "imgs/explosion_sheet.png"),
            r(sf::Vector2f(0, 100),
                sf::Vector2f(50.f * Env::pixpmeter / 1120, 50.f * Env::pixpmeter / 1120),
                0,
                1120,
                sf::Vector2f(0, 0),
                sf::Vector2f(0, 0),
                0,
                0,
                Gif(3, exframes),
                Rocket::Status::Regular)
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
        int get_window_width() const { return Env::ww; }
        int get_window_height() const { return Env::wh; }
    private:
        Frames exframes;
        Rocket r;
        Floor f;
        CollisionManager cm;

        void drawAll(sf::RenderWindow& win) const {
            win.draw(f);
            win.draw(*f.getBoundingBox().release());
            if (r.getStatus() != Rocket::Status::Explode && r.getStatus() != Rocket::Status::BlewUp)
                win.draw(*r.getBoundingBox().release());
            win.draw(r);
        }
};