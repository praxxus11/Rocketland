#pragma once 

#include <SFML/Graphics.hpp>
#include <utility>

class Env {
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
    template<typename T>
    static sf::Vector2<T> metersToPixels(const sf::Vector2<T>& pr) {
        sf::Vector2<T> res{};
        res.x = (pr.x * pixpmeter) + displ.x;
        res.y = (-pr.y * pixpmeter) + displ.y;
        return res;
    }

    // converts (pixel, pixel) to real life coordinates (meter, meter)
    template<typename T>
    static sf::Vector2<T> pixelsToMeters(const sf::Vector2<T>& pr) {
        sf::Vector2<T> res{};
        res.x = (pr.x - displ.x) / pixpmeter;
        res.y = -((pr.y - displ.y) / pixpmeter);
        return res;
    }

    // converts size (meter, meter) to (pixel, pixel)
    template<typename T>
    static sf::Vector2<T> convertSize(const sf::Vector2<T>& pr) {
        return sf::Vector2<T>(pr.x*pixpmeter, pr.y*pixpmeter);
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

int Env::ww = 1100;
int Env::wh = 800;
int Env::floor_hei = 100;
float Env::pixpmeter = 2;
const float Env::gravity = -9.8;
const float Env::PI = 3.14159265;
sf::Clock Env::clock{};
sf::Vector2f Env::displ(Env::ww/2, Env::wh - Env::floor_hei);