#pragma once 

#include <SFML/Graphics.hpp>
#include <array>
#include <random>

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
    static sf::Vector2f origin; // UNITS(x: pixels, y: pixels): 
                        // originacement of origin relative to top left corner
                        // down: positive, right: positive
    static sf::Vector2f camera_pos; // (meters, meters) that represent the center of the screen

    // converts real life coordinates (meter, meter) to (pixel, pixel)
    template<typename T>
    static sf::Vector2<T> metersToPixels(const sf::Vector2<T>& pr) {
        sf::Vector2<T> res{};
        res.x = (pr.x * pixpmeter) + origin.x - (camera_pos.x * pixpmeter);
        res.y = (-pr.y * pixpmeter) + origin.y + (camera_pos.y * pixpmeter);
        return res;
    }

    // converts (pixel, pixel) to real life coordinates (meter, meter)
    template<typename T>
    static sf::Vector2<T> pixelsToMeters(const sf::Vector2<T>& pr) {
        sf::Vector2<T> res{};
        res.x = (pr.x - origin.x + camera_pos.x * pixpmeter) / pixpmeter;
        res.y = -((pr.y - origin.y - camera_pos.y * pixpmeter) / pixpmeter);
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
    // static float g_elapsed() { return clock.getElapsedTime().asSeconds(); }
    static float g_elapsed() { return 0.05; }
    // static float g_elapsed() { return clock.getElapsedTime().asSeconds(); }

    static void restartc() { clock.restart(); }

    static int cycle_num;
    static int num_rocks;

    // ranadom numbers
    static std::random_device seed;
    static std::mt19937 rng;
    static std::uniform_int_distribution<int> rnd;
    static std::array<int, 2> interval_grad;
    static std::array<int, 2> weight_grad;
    static std::piecewise_linear_distribution<float> grad_rnd;
    static int get_rand() {
        return rnd(rng);
    }
    static float get_grad_rand() {
        return grad_rnd(rng);
    }
};

int Env::ww = 1100;
int Env::wh = 900;
int Env::floor_hei = wh/2;
float Env::pixpmeter = 7;
const float Env::gravity = -9.8;
const float Env::PI = 3.14159265;
sf::Clock Env::clock{};
sf::Vector2f Env::origin(Env::ww/2, Env::wh/2); // (position (0, 0))
sf::Vector2f Env::camera_pos(0, 0);
int Env::cycle_num = 0;
int Env::num_rocks = 1;

std::random_device Env::seed;
std::mt19937 Env::rng(Env::seed());
std::uniform_int_distribution<int> Env::rnd(0, INT_MAX);

std::array<int, 2> Env::interval_grad {0, Env::num_rocks/3};
std::array<int, 2> Env::weight_grad {1, 0};
std::piecewise_linear_distribution<float> Env::grad_rnd(interval_grad.begin(), interval_grad.end(), weight_grad.begin());
