#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "NeuralNetwork.h"
#include "RocketManager.h"

class AIManager {
public:
    AIManager() : 
        network(std::vector<int>{14, 16, 16, 8}, 
        std::vector<NeuralNetwork::ActivationFuncs>{
            NeuralNetwork::ActivationFuncs::none,
            NeuralNetwork::ActivationFuncs::none,
            NeuralNetwork::ActivationFuncs::tanh
        })
    {   
    }

    void init(std::vector<Rocket>& rockets) {
        for (Rocket& rocket : rockets) {
            networks.emplace_back(
                &rocket,
                network.get_random_weights_biases()
            );
        }
    }
    void update_rockets() {
        for (RocketManager& rm : networks) {
            rm.update_rocket(network);
        }
    }
private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;
};