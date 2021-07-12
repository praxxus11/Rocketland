#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "Env.h"

class Frames {
public:
    Frames(int sw, int sh, int frms, std::string sheet_loc) :
        sheet_width(sw),
        sheet_height(sh),
        number_frames(frms),
        sprites(number_frames)
    {
        std::cout << "Hello";
        try {
            sprite_sheet.loadFromFile(sheet_loc);
            const int imghei = sheet_height / number_frames;
            for (int i=0; i<number_frames; i++) {
                sprites[i] = sf::Sprite(sprite_sheet, sf::IntRect(0, i*imghei, sheet_width, imghei));
            }
        }
        catch (...) {
            std::cout << "Sprite sheet png not loaded";
        }
    }
    sf::Sprite operator[](int i) const {
        return sprites[i];
    }
    int number_of_frames() const { return number_frames; }
private:
    sf::Texture sprite_sheet;
    int number_frames;
    int sheet_width;
    int sheet_height;
    std::vector<sf::Sprite> sprites;
};

class Gif : public GameObject {
public:
    Gif(float dur, Frames& spr_frms) :
        GameObject({0, 0}, {1, 1}, 0), 
        duration(dur),
        time_elapsed(0),
        curr_frame(-1),
        on_new_frame(0),
        sprite_frames(spr_frms)
    {
    }
    ~Gif() 
    {
    }
    void update() {
        if (curr_frame >= number_frames()) return;
        else if (curr_frame == -1) curr_frame = 0;
        else {
            float req_elapsed = duration / number_frames();
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
        sf::FloatRect ir = sprite_frames[0].getLocalBounds();
        ir = getTransform().transformRect(ir);
        const sf::Vector2f newcor = Env::pixelsToMeters(sf::Vector2f(ir.left, ir.top));
        return sf::FloatRect(newcor.x, newcor.y, ir.width/Env::pixpmeter, ir.height/Env::pixpmeter);
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (curr_frame >= number_frames() || curr_frame < 0) return; 
        states.transform *= getTransform();
        // target.draw(sprites[curr_frame], states);
        target.draw(sprite_frames[curr_frame], states);
    }
    int number_frames() const { return sprite_frames.number_of_frames(); }
    int get_curr() const { return curr_frame; }
    bool ison_new_frame() const { return on_new_frame; }
private:
    int curr_frame;
    float time_elapsed;
    float duration;
    bool on_new_frame;
    Frames& sprite_frames;
};