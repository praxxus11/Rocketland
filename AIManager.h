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
            NeuralNetwork::ActivationFuncs::relu,
            NeuralNetwork::ActivationFuncs::tanh,
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
        bool all_done = 1;
        for (RocketManager& rm : networks) {
            rm.update_rocket(network);
            all_done = all_done && (rm.is_crashed() || rm.is_landed());
        }
        if (all_done) {
            for (RocketManager& rm : networks) {
                rm.updateScore();
            }
            sort(networks.begin(), networks.end(), [](const RocketManager& a, const RocketManager& b) {
                return (a.getScore() < b.getScore());
            });

            for (RocketManager& rm : networks) {
                rm.reset();
            }
        }
    }

    void do_cross_over(RocketManager& a, RocketManager& b) {

    }

    void do_mutations(RocketManager& rm) {

    }

private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;
};