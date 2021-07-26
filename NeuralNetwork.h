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
        activation_funcs(a_funcs),
        weights_ct(0),
        biases_ct(0)
    {
        assert(layer_sizes.size() == 1+activation_funcs.size());
        for (int i=0; i<layer_sizes.size() - 1; i++) {
            weights_ct += layer_sizes[i] * layer_sizes[i + 1];
            biases_ct += layer_sizes[i + 1];
        }
    }

    void fill_random(float* arr, int n) {
        for (int i=0; i<n; i++) {
            arr[i] = float(2 * (float(Env::get_rand()) / INT_MAX) - 1); // -1 to 1
        }
    }

    std::vector<std::vector<Eigen::MatrixXf>> get_wb_fromfile(std::string filename) {
        std::ifstream fin(filename);
        std::vector<std::vector<Eigen::MatrixXf>> res;
        for (int num=0; num<30; num++) {
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

    void front_prop(float* input_vector, float* output_vector, float* weights, float* biases) const {
        Eigen::Map<Eigen::MatrixXf> activations(input_vector, 1, layer_sizes[0]);
        Eigen::MatrixXf activations_ref = activations;
        std::vector<Eigen::MatrixXf> weights_vec;
        std::vector<Eigen::MatrixXf> biases_vec;
        int num_weights = 0;
        int num_biases = 0;
        for (int i=0; i < layer_sizes.size() - 1; i++) {
            Eigen::Map<Eigen::MatrixXf> weight(weights + num_weights, layer_sizes[i + 1], layer_sizes[i]);
            Eigen::Map<Eigen::MatrixXf> bias(biases + num_biases, 1, layer_sizes[i + 1]);
            num_weights += layer_sizes[i + 1] * layer_sizes[i];
            num_biases += layer_sizes[i + 1];
            weights_vec.push_back(weight.transpose()); // because matrixes are stored in column major order in eigen
            biases_vec.push_back(bias);
        } 

        for (int i=0; i<layer_sizes.size() - 1; i++) {
            activations_ref *= weights_vec[i];
            activations_ref += biases_vec[i];
            switch (activation_funcs[i]) {
                case ActivationFuncs::relu:
                    activations_ref = activations_ref.unaryExpr(relu_ff);
                    break;
                case ActivationFuncs::tanh:
                    activations_ref = activations_ref.unaryExpr(tanh_ff);
                defualt:
                    break;
            }
        }
        output_vector[0] = activations_ref.data()[0];
        output_vector[1] = activations_ref.data()[1];
    }

    const std::vector<int>& get_layer_sizes() const {
        return layer_sizes;
    }

    int get_weights_ct() const {
        return weights_ct;
    }

    int get_biases_ct() const {
        return biases_ct;
    }

private:
    std::function<float(float)> relu_ff = [](float x) { return std::max(0.f, x); };
    std::function<float(float)> tanh_ff = [](float x) { return tanh(x); };
    std::vector<int> layer_sizes;
    int weights_ct;
    int biases_ct;
    std::vector<ActivationFuncs> activation_funcs;
};