#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Settings.h"

class Gif : public sf::Drawable, public sf::Transformable {
public:
    Gif(int sw, int sh, int frms, float dur) : 
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
            time_elapsed += Settings::g_elapsed();
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
    void reset() {
        if (curr_frame >= frames) curr_frame = -1;
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