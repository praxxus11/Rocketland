#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <assert.h>
#include <math.h>
#include <functional>
#include <string>
#include <fstream>

class NeuralNetwork {
public:
    enum class ActivationFuncs {
        none, tanh, relu
    };
    NeuralNetwork(std::vector<int> l_sizes,
        std::vector<ActivationFuncs> a_funcs
    ) : layer_sizes(l_sizes),
        activation_funcs(a_funcs)
    {
        assert(layer_sizes.size() == 1+activation_funcs.size());
    }

    std::vector<Eigen::MatrixXf> get_random_weights_biases() {
        std::vector<Eigen::MatrixXf> res;
        for (int i=0; i<layer_sizes.size()-1; i++) {
            // weights & bias from layer i to layer (i + 1)
            // the last row are the biases for the layer
            res.push_back(Eigen::MatrixXf::Random(layer_sizes[i] + 1, layer_sizes[i+1]));
        }
        return res;
    }

    std::vector<std::vector<Eigen::MatrixXf>> get_wb_fromfile(std::string filename) {
        std::ifstream fin("saves/save_cycle450.txt");
        std::vector<std::vector<Eigen::MatrixXf>> res;
        for (int num=0; num<29; num++) {
            std::vector<Eigen::MatrixXf> temp;
            for (int layer=0; layer<layer_sizes.size()-1; layer++) {
                Eigen::MatrixXf weights_biases(layer_sizes[layer]+1, layer_sizes[layer+1]);
                for (int i=0; i<layer_sizes[layer]+1; i++) {
                    for (int j=0; j<layer_sizes[layer+1]; j++) {
                        fin >> weights_biases(i, j);
                    }
                }
                temp.push_back(weights_biases);
            }
            res.push_back(temp);
        }
        return res;
    }

    std::vector<float> front_prop(const std::vector<float>& input_vector, const std::vector<Eigen::MatrixXf>& weights_biases) const {
        assert(input_vector.size() == layer_sizes[0]);
        assert(weights_biases.size() == layer_sizes.size() - 1);
        for (int i=0; i<weights_biases.size(); i++) {
            assert(weights_biases[i].rows() == layer_sizes[i] + 1);
            assert(weights_biases[i].cols() == layer_sizes[i+1]);
        }

        Eigen::MatrixXf activations(1, input_vector.size() + 1);
        for (int i=0; i<input_vector.size(); i++) {
            activations(0, i) = input_vector[i];
        }
        activations(0, input_vector.size()) = 1; // constant for the bias term 
        
        for (int i=0; i<weights_biases.size(); i++) {
            activations = activations * weights_biases[i];
            switch (activation_funcs[i]) {
                case ActivationFuncs::relu:
                    activations = activations.unaryExpr(relu_ff);
                    break;
                case ActivationFuncs::tanh:
                    activations = activations.unaryExpr(tanh_ff);
                defualt:
                    break;
            }
            if (i < weights_biases.size()-1) { // then append a constant 1 term for the bias
                activations.conservativeResize(1, activations.cols() + 1);
                activations(0, activations.cols()-1) = 1; // constant for the bias term
            }
        }
        return std::vector<float>(activations.data(), activations.data() + activations.size());
    }

private:
    std::function<float(float)> relu_ff = [](float x) { return std::max(0.f, x); };
    std::function<float(float)> tanh_ff = [](float x) { return tanh(x); };
    std::vector<int> layer_sizes;
    std::vector<ActivationFuncs> activation_funcs;
};