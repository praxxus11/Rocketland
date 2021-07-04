#pragma once 

#include <SFML/Graphics.hpp>
#include <utility>

class Settings {
public:
    static int ww;
    static int wh;  
    static int floor_hei;  
    
    static float pixpmeter;
    static float siscale;
    static float gravity;

    static sf::Clock clock;

    static float g_elapsed() {
        return clock.getElapsedTime().asSeconds();
    }
    static void restartc() {
        clock.restart();
    }
    static float time_so_far;

    // converts real life coordinates (meter, meter) to (pixel, pixel)
    static std::pair<float,float> convertUnits(const std::pair<float,float>& pr) {
    
    }
};

int Settings::ww = 800;
int Settings::wh = 800;
int Settings::floor_hei = 100;
float Settings::siscale = 1.f;
float Settings::pixpmeter = 6.6;
float Settings::gravity = -9.8;
sf::Clock Settings::clock;
float Settings::time_so_far = 0;