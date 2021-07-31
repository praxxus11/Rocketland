#include <SFML/Graphics.hpp>
#include <vector>

#include "Cloud.h"
#include "RocketStats.h"

class DecorationsManager {
public:
    DecorationsManager() {
        for (int i=3; i<60; i++) {
            int cloudx = (rand()%15000)-7500;
            int cloudy = (rand()%2000)+1000;
            clouds.emplace_back(sf::Vector2f(cloudx, cloudy), ResourceManger::ResourceTypes::Haze);
        }
    }
    void init_rocket_labels(std::vector<Rocket>& rockets) {
        for (Rocket& rocket : rockets) {
            labels.emplace_back(sf::Vector2f(0, 0), &rocket);
        }
    }
    void update() {
        for (Cloud& cloud : clouds) {
            cloud.irlSetPosition(cloud.irlGetPosition());
            cloud.setScale(sf::Vector2f(2000.f * Env::pixpmeter / 1120, 2000.f * Env::pixpmeter / 1120));
        }
        for (RocketStats& stat : labels) {
            stat.update();
        }
    }
    void draw(sf::RenderWindow& win) const {
        for (const Cloud& cloud : clouds) {
            win.draw(cloud);
        }
        for (const RocketStats& stat : labels) {
            win.draw(stat);
        }
    }
private:
    std::vector<Cloud> clouds;
    std::vector<RocketStats> labels;
};