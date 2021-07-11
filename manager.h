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
            // if (!cm.rocket_floor_collision(r, f)) 
                // r.setStatus(Rocket::Status::Regular);
            // else 
                // r.setStatus(Rocket::Status::Explode); // automaticcally changes to Rocket::Status::BlewUp

            r.update();

            // r.setScale(r.getScale()); // when you put on engines
            // r.setRotation(r.getRotation());
            drawAll(win);
            // std::cout << r.getPosition().y << " ";
        }
        int get_window_width() const { return Settings::ww; }
        int get_window_height() const { return Settings::wh; }
    private:
        Rocket r;
        Floor f;
        CollisionManager cm;

        void drawAll(sf::RenderWindow& win) const {
            sf::FloatRect bb = r.getGlobalBounds();
            sf::RectangleShape rect(Settings::convertSize(sf::Vector2f(bb.width, bb.height)));
            rect.setFillColor(sf::Color::Transparent);
            rect.setOutlineColor(sf::Color::Red);
            rect.setOutlineThickness(3.f);
            sf::Vector2f temp = Settings::metersToPixels(sf::Vector2f(bb.left, bb.top));
            rect.setPosition(temp.x, temp.y);
            win.draw(f);
            win.draw(r);
            // win.draw(rect);

            sf::Vertex a(Settings::displ, sf::Color::White);
            sf::Vertex b(sf::Vector2f(Settings::displ.x, 0), sf::Color::White);
            sf::Vertex vert[2] {a, b};
            win.draw(vert, 2, sf::Lines);
        }
};