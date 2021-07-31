#include <SFML/Graphics.hpp>
#include <vector>

#include "Cloud.h"

class DecorationsManager {
public:
    DecorationsManager() {
        for (int i=3; i<60; i++) {
            int cloudx = (rand()%15000)-7500;
            int cloudy = (rand()%2000)+1400;
            clouds.emplace_back(sf::Vector2f(cloudx, cloudy), ResourceManger::ResourceTypes::Haze);
        }
    }
    void update() {
        for (Cloud& cloud : clouds) {
            cloud.irlSetPosition(cloud.irlGetPosition());
            cloud.setScale(sf::Vector2f(2000.f * Env::pixpmeter / 1120, 2000.f * Env::pixpmeter / 1120));
        }
    }
    void draw(sf::RenderWindow& win) const {
        for (const Cloud& cloud : clouds) {
            win.draw(cloud);
        }
    }
private:
    std::vector<Cloud> clouds;
};