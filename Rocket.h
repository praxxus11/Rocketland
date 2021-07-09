#include <SFML/Graphics.hpp>
#include <iostream>
#include <utility>

class Rocket : public sf::Drawable, public sf::Transformable {
public:
    Rocket() : rotation(10.f),
        pixels_tall(1120),
        scale(50.f * Settings::pixpmeter / pixels_tall, 50.f * Settings::pixpmeter / pixels_tall),
        position(0, 100),
        vel(0, 0),
        accel(0, 0),
        explosion_anim(128, 3328, 26, 3)
    {
        if (!texture.loadFromFile("imgs/ship.png")) {
            std::cout << "Rocket png not loaded";
        }
        texture.setSmooth(true);
        sprite.setTexture(texture);
        explosion_anim.setOrigin(72, 120);
    }
    enum class Status {
        Regular, Explode
    };
    sf::FloatRect getGlobalBounds() const {
        return getTransform().transformRect(sprite.getGlobalBounds());
    }
    void update(Status status) {
        switch (status) {
        case Status::Regular: {
            float elap = Settings::g_elapsed();

            vel.x += accel.x*elap;
            vel.y += Settings::gravity*elap;
            
            sf::Vector2f newPos {Settings::convertUnits<float>(sf::Vector2f(position.x, position.y))};
            setPosition(newPos.x, newPos.y);
            position.x += vel.x*elap;
            position.y += vel.y*elap;
            break;
        }
        case Status::Explode: {
            if (!explosion_initialized) {
                const auto globalbnd = getGlobalBounds();
                explosion_anim.setPosition(globalbnd.left + 0.5*globalbnd.width, globalbnd.top + globalbnd.height);
                constexpr float times_bigger = 1.3; // how large explosion is compared to rocket
                const float scale = (times_bigger * 1120 * getScale().x) / 128;
                explosion_anim.setScale(scale, scale);
                explosion_initialized = 1;
            }
            explosion_anim.update();
            if (explosion_anim.get_curr() <= explosion_anim.get_frames() && 
                !(explosion_anim.get_curr() % (explosion_anim.get_frames() / 5)) &&
                explosion_anim.ison_new_frame()) {
                std::cout << "Hello!!!";
                setPosition(getPosition().x, getPosition().y + getPosition().y / 5);
            }
            break;
        }
        } // end switch (status)
    } 
    int pixels_tall; // change this code later
    float rotation;
    sf::Vector2f scale;
    sf::Vector2f position; 
    sf::Vector2f vel;
    sf::Vector2f accel;
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(sprite, states);
        target.draw(explosion_anim);

    }
    sf::Texture texture;
    sf::Sprite sprite;
    Gif explosion_anim;
    bool explosion_initialized = 0;
};