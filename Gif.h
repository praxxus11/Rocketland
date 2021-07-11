#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "Env.h"

class Frames {
public:
    Frames(int sw, int sh, int frms, std::string sheet_loc) :
        number_frames(frms),
        sheet_width(sw),
        sheet_height(sh),
        sprites(number_frames)
    {
        sf::Texture sprite_sheet;
        try {
            sprite_sheet.loadFromFile("imgs/explosion_sheet.png");
            const int imghei = sheet_height / number_frames;
            for (int i=0; i<number_frames; i++) {
                sprites.push_back(
                    sf::Sprite(sprite_sheet, sf::IntRect(0, i*imghei, sheet_width, imghei)));
            }
        }
        catch (...) {
            std::cout << "Sprite sheet png not loaded";
        }
    }
    const sf::Sprite& operator[](int i) const {
        return sprites[i];
    }
private:
    int number_frames;
    int sheet_width;
    int sheet_height;
    std::vector<sf::Sprite> sprites;
};

class Gif : public GameObject {
public:
    Gif(int sw, int sh, int frms, float dur) :
        GameObject({0, 0}, {1, 1}, 0), 
        sheet_width(sw),
        sheet_height(sh),
        frames(frms),
        duration(dur),
        time_elapsed(0),
        curr_frame(-1),
        on_new_frame(0)
    {
        try {
            sprite_sheet.loadFromFile("imgs/explosion_sheet.png");
            const int imghei = sheet_height / frames;
            for (int i=0; i<frames; i++) {
                sprites.push_back(
                    sf::Sprite(sprite_sheet, sf::IntRect(0, i*imghei, sheet_width, imghei)));
            }
        }
        catch (...) {
            std::cout << "Sprite sheet png not loaded";
        }
    }
    ~Gif() 
    {
    }
    void update() {
        if (curr_frame >= frames) return;
        else if (curr_frame == -1) curr_frame = 0;
        else {
            float req_elapsed = duration / frames;
            time_elapsed += Env::g_elapsed();
            if (time_elapsed > req_elapsed) {
                curr_frame += time_elapsed / req_elapsed;
                time_elapsed = 0;
                on_new_frame = 1;
            } 
            else {
                on_new_frame = 0;
            }
        }
    }

    sf::FloatRect getGlobalBounds() const override {
        sf::FloatRect ir = sprites[0].getLocalBounds();
        ir = getTransform().transformRect(ir);
        const sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (curr_frame >= frames || curr_frame < 0) return; 
        states.transform *= getTransform();
        target.draw(sprites[curr_frame], states);
    }
    int get_frames() const { return frames; }
    int get_curr() const { return curr_frame; }
    bool ison_new_frame() const { return on_new_frame; }
private:
    sf::Texture sprite_sheet; // assume that the sprite sheet has 1 image per row
    std::vector<sf::Sprite> sprites;
    int sheet_width;
    int sheet_height;
    int frames;
    int curr_frame;
    float time_elapsed;
    float duration;
    bool on_new_frame;
};