#include <iostream>

#include "Settings.h"
#include "Gif.h"
#include "Rocket.h"
#include "Floor.h"
#include "CollisionManager.h"

class Manager {
    public:
        Manager() 
        {
            sf::Vector2f newPos {Settings::convertUnits<float>(sf::Vector2f(r.position.x, r.position.y))};
            r.setPosition(newPos.x, newPos.y);
            r.setScale(r.scale.x, r.scale.y);
            r.setRotation(r.rotation);
        }
        ~Manager() 
        {
        }
        void update(sf::RenderWindow& win) {

            if (!cm.rocket_floor_collision(r, f)) 
                r.update(Rocket::Status::Regular);
            else 
                r.update(Rocket::Status::Explode); 

            r.setScale(r.scale.x, r.scale.y);
            r.setRotation(r.rotation);
            drawAll(win);
        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;
        CollisionManager cm;

        void drawAll(sf::RenderWindow& win) const {
            sf::FloatRect bb = r.getGlobalBounds();
            sf::RectangleShape rect(sf::Vector2f(bb.width, bb.height));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Red);
            rect.setOutlineThickness(3.f);
            rect.setPosition(bb.left, bb.top);
            win.draw(f);
            win.draw(r);
            win.draw(rect);

            sf::Vertex a(Settings::displ, sf::Color::White);
            sf::Vertex b(sf::Vector2f(Settings::displ.x, 0), sf::Color::White);
            sf::Vertex vert[2] {a, b};
            win.draw(vert, 2, sf::Lines);
        }
};