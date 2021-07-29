#include <SFML/Graphics.hpp>
#include <Eigen/Dense>
#include <vector>
#include <iostream>
#include <assert.h>
#include <math.h>
#include <functional>
#include <string>
#include <fstream>

#if defined(GPU)
    #include "Cuda/matmul.h"
#endif

class NeuralNetwork {
public:
    enum class ActivationFuncs {
        none, tanh, relu
    };
    NeuralNetwork(std::vector<int> l_sizes,
        std::vector<ActivationFuncs> a_funcs
    ) : layer_sizes(l_sizes),
        activation_funcs(a_funcs)
#if defined(GPU)
        ,weights_ct(0),
        biases_ct(0)
#endif
    {
        assert(layer_sizes.size() == 1+activation_funcs.size());
#if defined(GPU)
        for (int i=0; i<layer_sizes.size() - 1; i++) {	
            weights_ct += layer_sizes[i] * layer_sizes[i + 1];	
            biases_ct += layer_sizes[i + 1];	
        }
#endif
    }

#if defined(CPU)
    std::vector<Eigen::MatrixXf> get_random_weights_biases() {
        std::vector<Eigen::MatrixXf> res;
        for (int i=0; i<layer_sizes.size()-1; i++) {
            // weights & bias from layer i to layer (i + 1)
            // the last row are the biases for the layer
            res.push_back(Eigen::MatrixXf::Random(layer_sizes[i] + 1, layer_sizes[i+1]));
        }
        return res;
    }
#elif defined(GPU)
    void fill_random(float* arr, int n) {	
        for (int i=0; i<n; i++) {	
            arr[i] = float(2 * (float(Env::get_rand()) / INT_MAX) - 1); // -1 to 1	
        }	
    }
#endif

#if defined(CPU)
    std::vector<std::vector<Eigen::MatrixXf>> get_wb_fromfile(std::string filename) {
        std::ifstream fin(filename);
        int weightspr = 0;
        int biasespr = 0;
        for (int i=0; i<layer_sizes.size()-1; i++) {
            weightspr += layer_sizes[i] * layer_sizes[i+1];
            biasespr += layer_sizes[i+1];
        }
        const int size = Env::load_rocks/10;
        float* weights = new float[size * weightspr];
        float* biases = new float[size * biasespr];
        for (int i=0; i<size; i++) {
            for (int j=0; j<weightspr; j++) fin >> weights[weightspr * i + j];
            for (int j=0; j<biasespr; j++) fin >> biases[biasespr * i + j];
        }
        int w = 0, b = 0;
        std::vector<std::vector<Eigen::MatrixXf>> res;
        for (int num=0; num<size; num++) {
            std::vector<Eigen::MatrixXf> temp;
            for (int layer=0; layer<layer_sizes.size()-1; layer++) {
                Eigen::MatrixXf weights_biases(layer_sizes[layer]+1, layer_sizes[layer+1]);
                for (int i=0; i<layer_sizes[layer]+1; i++) {
                    for (int j=0; j<layer_sizes[layer+1]; j++) {
                        if (i == layer_sizes[layer]) {
                            weights_biases(i, j) = biases[b++];
                        }
                        else {
                            weights_biases(i, j) = weights[w++];
                        }
                    }
                }
                temp.push_back(weights_biases);
            }
            res.push_back(temp);
        }
        delete[] weights;
        delete[] biases;
        return res;
    }

#elif defined(GPU)
    void fill_from_file(float* weights, float* biases, std::string filename) {	
        std::ifstream fin(filename);	
        const int tot_saved = Env::load_rocks/10;	
        float* weight_temp = new float[tot_saved * get_weights_ct()];	
        float* bias_temp = new float[tot_saved * get_biases_ct()];	
        for (int r=0; r<tot_saved; r++) {	
            for (int i=0; i<get_weights_ct(); i++) fin >> weight_temp[r * get_weights_ct() + i];	
            for (int i=0; i<get_biases_ct(); i++) fin >> bias_temp[r * get_biases_ct() + i];	
        }	
        for (int r=0; r<Env::num_rocks; r++) {	
            const int prev_weights_ct = r * get_weights_ct();	
            const int prev_biases_ct = r * get_biases_ct();	
            const int weights_stored = tot_saved * get_weights_ct();	
            const int biases_stored = tot_saved * get_biases_ct();	
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
#endif

#if defined(CPU)
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
            activations *= weights_biases[i];
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

#elif defined(GPU)
    void front_prop(float* input_vector, float* output_vector, float* weights, float* biases)  {	
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
#endif

private:
    std::function<float(float)> relu_ff = [](float x) { return std::max(0.f, x); };
    std::function<float(float)> tanh_ff = [](float x) { return tanh(x); };
    std::vector<int> layer_sizes;
    std::vector<ActivationFuncs> activation_funcs;
#if defined(GPU)
    int weights_ct;	
    int biases_ct;
#endif
};