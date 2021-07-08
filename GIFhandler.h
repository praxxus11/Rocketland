#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

#include "Settings.h"

class GIFhandler : public sf::Drawable, public sf::Transformable {
public:
    GIFhandler(int sw, int sh, int frms, float dur) : 
        sheet_width(sw),
        sheet_height(sh),
        frames(frms),
        duration(dur),
        time_elapsed(0),
        curr_frame(0)
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
    ~GIFhandler() 
    {
    }
    void update() {
        if (curr_frame >= frames || curr_frame == -1);
        else {
            float req_elapsed = duration / frames;
            time_elapsed += Settings::g_elapsed();
            if (time_elapsed > req_elapsed) {
                curr_frame += time_elapsed / req_elapsed;
                time_elapsed = 0;
            }
        }
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        if (curr_frame >= frames) return; 
        states.transform *= getTransform();
        target.draw(sprites[curr_frame], states);
    }
private:
    sf::Texture sprite_sheet;
    std::vector<sf::Sprite> sprites;
    int sheet_width;
    int sheet_height;
    int frames;
    float time_elapsed;
    float curr_frame;
    float duration;
};