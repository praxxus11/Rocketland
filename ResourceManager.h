#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>

class ResourceManger {
public:
    enum class ResourceTypes {
        RocketImg
    };
    ResourceManger() 
    {
        filepaths[ResourceTypes::RocketImg] = "imgs/ship.png";
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
        if (instance == nullptr) 
            instance = new ResourceManger();
        return instance;
    }
private:
    std::map<ResourceTypes, std::string> filepaths;
    std::map<ResourceTypes, sf::Texture> textures;
    static ResourceManger* instance;
};

ResourceManger* ResourceManger::instance = nullptr;