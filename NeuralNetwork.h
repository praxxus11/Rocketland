#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <assert.h>

class NeuralNetwork {
public:
    enum class ActivationFuncs {
        none, tanh, relu, leakyrelu
    };
    NeuralNetwork(std::vector<int> l_sizes,
        std::vector<ActivationFuncs> a_funcs
    ) : layer_sizes(l_sizes),
        activation_funcs(a_funcs)
    {
        assert(layer_sizes.size() == 1+activation_funcs.size());
    }
    std::vector<float> getResult() {
        return std::vector<float>();
    }
private:
    std::vector<int> layer_sizes;
    std::vector<ActivationFuncs> activation_funcs;
};