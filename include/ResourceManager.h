#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>

// singleton class
class ResourceManger {
public:
    enum class ResourceTypes {
        RocketImg, RocketFlame, RocketUpperFin, RocketLowerFin
    };
    ResourceManger() 
    {
        filepaths[ResourceTypes::RocketImg] = "imgs/ship_no_fins.png";
        filepaths[ResourceTypes::RocketFlame] = "imgs/flame.png";
        filepaths[ResourceTypes::RocketLowerFin] = "imgs/rocket_lower_fin.png";
        filepaths[ResourceTypes::RocketUpperFin] = "imgs/rocket_upper_fin.png";
    }
    ~ResourceManger() {
        delete instance;
    }
    sf::Texture& getTexture(ResourceTypes resource) {
        const auto& textureFound = textures.find(resource);
        if (textureFound == textures.end()) {
            sf::Texture& texture = textures[resource];
            texture.loadFromFile(filepaths[resource]);
            texture.setSmooth(true);
            return texture;
        }
        else {
            return textureFound->second;
        }
    }
    static ResourceManger* getInstance() {
        if (!instance) 
            instance = new ResourceManger();
        return instance;
    }
private:
    std::map<ResourceTypes, std::string> filepaths;
    std::map<ResourceTypes, sf::Texture> textures;
    static ResourceManger* instance;
};

ResourceManger* ResourceManger::instance = nullptr;