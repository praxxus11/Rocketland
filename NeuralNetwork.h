#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <assert.h>
#include <math.h>
#include <functional>
#include <string>
#include <fstream>

#include "../Cuda/matmul.h"

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

    void fill_from_file(float* weights, float* biases, std::string filename) {
        std::ifstream fin(filename);
        float* weight_temp = new float[30 * get_weights_ct()];
        float* bias_temp = new float[30 * get_biases_ct()];
        for (int r=0; r<30; r++) {
            for (int i=0; i<get_weights_ct(); i++) fin >> weight_temp[r * get_weights_ct() + i];
            for (int i=0; i<get_biases_ct(); i++) fin >> bias_temp[r * get_biases_ct() + i];
        }
        for (int r=0; r<Env::num_rocks; r++) {
            const int prev_weights_ct = r * get_weights_ct();
            const int prev_biases_ct = r * get_biases_ct();
            const int weights_stored = 30 * get_weights_ct();
            const int biases_stored = 30 * get_biases_ct();
            for (int i=0; i<get_weights_ct(); i++) {
                weights[prev_weights_ct + i] = weight_temp[(prev_weights_ct + i) % weights_stored];
            }
            for (int i=0; i<get_biases_ct(); i++) {
                biases[prev_biases_ct + i] = bias_temp[(prev_biases_ct + i) % biases_stored];
            }
        }
        delete[] weight_temp;
        delete[] bias_temp;
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
        matmul(weights, biases, input_vector, output_vector, layer_sizes.size(), layer_sizes.data(), Env::num_rocks);
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