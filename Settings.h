#pragma once 

#include <SFML/Graphics.hpp>
#include <utility>

class Settings {
public:
    /*
        these are keeping track of meta-dimensions
    */

    static int ww; // UNITS(pixels): width of the screen
    static int wh; // UNITS(pixels): height of the screen

    /*
        these are keepin track of dimensions
    */

    static int floor_hei; // UNITS(pixels): height of the floor

    /*
        these are responsible for transforming UNITS(meters) => UNITS(pixels)
    */

    static float pixpmeter; // UNITS(pixels/meter)
    static sf::Vector2f displ; // UNITS(x: pixels, y: pixels): 
                        // displacement of origin relative to top left corner
                        // down: positive, right: positive

    // converts real life coordinates (meter, meter) to (pixel, pixel)
    static sf::Vector2f convertUnits(const sf::Vector2f& pr) {
        sf::Vector2f res{};
        res.x = (pr.x * pixpmeter) + displ.x;
        res.y = (-pr.y * pixpmeter) + displ.y;
        return res;
    }

    // converts size (meter, meter) to (pixel, pixel)
    static sf::Vector2f convertSize(const sf::Vector2f& pr) {
        return sf::Vector2f(pr.x*pixpmeter, pr.y*pixpmeter);
    }

    /*
        scientific constants
    */
    static const float gravity; // UNITS(m/s^2): downard acceleration
    static const float PI;

    /* 
        handling time
    */

    static sf::Clock clock;
    static float g_elapsed() { return clock.getElapsedTime().asSeconds(); }
    static void restartc() { clock.restart(); }
};

int Settings::ww = 800;
int Settings::wh = 800;
int Settings::floor_hei = 100;
float Settings::pixpmeter = 2;
const float Settings::gravity = -9.8;
const float Settings::PI = 3.14159265;
sf::Clock Settings::clock{};
sf::Vector2f Settings::displ(Settings::ww/2, Settings::wh - Settings::floor_hei);