#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "NeuralNetwork.h"

class AIManager {
public:
    AIManager() : network(std::vector<int>{1}, std::vector<NeuralNetwork::ActivationFuncs>{})
    {
    }
private:
    NeuralNetwork network;
};