#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <Eigen/Dense>

#include "NeuralNetwork.h"
#include "RocketManager.h"

class AIManager {
public:
    AIManager() : 
        network(std::vector<int>{8, 4, 2}, 
        std::vector<NeuralNetwork::ActivationFuncs>{
            NeuralNetwork::ActivationFuncs::tanh,
            NeuralNetwork::ActivationFuncs::tanh
        })
    {   
        const auto& layers = network.get_layer_sizes();
        rocket_inputs = new float[layers[0] * Env::num_rocks];
        rocket_outputs = new float[layers[layers.size()-1] * Env::num_rocks];

        weights = new float[network.get_weights_ct() * Env::num_rocks];
        biases = new float[network.get_biases_ct() * Env::num_rocks];
    }
    ~AIManager() {
        delete[] rocket_inputs;
        delete[] rocket_outputs;
        delete[] weights;
        delete[] biases;
    }

    void init_random(std::vector<Rocket>& rockets) {
        for (int i=0; i<rockets.size(); i++) {
            networks.emplace_back(&rockets[i], i);
        }
        network.fill_random(weights, network.get_weights_ct() * Env::num_rocks);
        network.fill_random(biases, network.get_biases_ct() * Env::num_rocks);
    }
    
    void init_from_file(std::vector<Rocket> rockets, std::string filename) {
        for (int i=0; i<rockets.size(); i++) {
            networks.emplace_back(&rockets[i], i);
        }
        network.fill_from_file(weights, biases, filename);
    }

    void print_arr(float* arr, int n) {
        for (int i=0; i<n; i++) std::cout << arr[i] << " ";
        std::cout << '\n';
    }
    void update_rockets() {
        int all_done = 1;
        for (int i=0; i<networks.size(); i++) {
            networks[i].update_inputs(&rocket_inputs[i * 8]);

            /////////////////////////////////
            // here should be convert to gpu
            network.front_prop(
                &rocket_inputs[i * 8], 
                &rocket_outputs[i * 2],
                &weights[i * network.get_weights_ct()], 
                &biases[i * network.get_biases_ct()]
            );
            // ////////////////////////////////

            networks[i].update_outputs(&rocket_outputs[i * 2]);
            all_done += (networks[i].is_crashed() || networks[i].is_landed());
        }

        if (all_done >= Env::num_rocks) {
            Env::cycle_num++;
            double tot = 0;
            int ct = 0;
            for (RocketManager& rm : networks) {
                rm.updateScore();
                if (rm.getScore() < 5e6) {
                    tot += rm.getScore();
                    ct++;
                }
            }
            std::cout << "Iteration: " << Env::cycle_num << " Average score: " << tot/ct << "\n";
            
            sort(networks.begin(), networks.end(), [](const RocketManager& a, const RocketManager& b) {
                return (a.getScore() < b.getScore());
            });

            if (Env::cycle_num%50==0) {
                std::cout << "\n\nSaving...\n\n";
                std::ofstream fout("C:/Users/Eric/ProgrammingProjectsCpp/RocketSaves/cycle_num" + std::to_string(Env::cycle_num) + ".txt");
                for (int r=0; r<Env::num_rocks/10; r++) {
                    for (int i=0; i<network.get_weights_ct(); i++) {
                        fout << weights[networks[r].get_index() * network.get_weights_ct() + i] << " "; 
                    }
                    for (int i=0; i<network.get_biases_ct(); i++) {
                        fout << biases[networks[r].get_index() * network.get_biases_ct() + i] << " ";
                    }
                }
                fout << "\n\n";
            }

            do_cross_over(0.4, networks);
            do_mutations(0.04*exp(-0.01*Env::cycle_num) + 0.006);

            for (int i=0; i<networks.size(); i++) {
                networks[i].reset();
                networks[i].set_index(i);
            }

        }
    }

    void do_cross_over(float cross_over_chance, const std::vector<RocketManager>& mans) {
        const auto& layers = network.get_layer_sizes();
        float* temp_weights = new float[network.get_weights_ct() * Env::num_rocks];
        float* temp_biases = new float[network.get_biases_ct() * Env::num_rocks];

        for (int r=0; r<Env::num_rocks; r++) { // need to fill temp with number of rockets
            int a = mans[round(Env::get_grad_rand())].get_index();
            int b = mans[round(Env::get_grad_rand())].get_index();
            float* a_weights = &weights[a * network.get_weights_ct()];
            float* b_weights = &weights[b * network.get_weights_ct()];
            float* a_biases = &biases[a * network.get_biases_ct()];
            float* b_biases = &biases[b * network.get_biases_ct()];
            int weights_so_far = 0;
            int biases_so_far = 0;
            for (int l=0; l<layers.size()-1; l++) {
                int cols = layers[l+1];
                int rows = layers[l];

                // doing biases
                for (int col=0; col<cols; col++) {
                    if (Env::get_rand()%2) {
                        temp_biases[
                            r * network.get_biases_ct() + 
                            biases_so_far + 
                            col
                        ] = a_biases[
                            biases_so_far + 
                            col
                        ];
                    }
                    else {
                        temp_biases[
                            r * network.get_biases_ct() + 
                            biases_so_far + 
                            col
                        ] = b_biases[
                            biases_so_far + 
                            col
                        ];                   
                    }
                }

                // doing weights
                for (int col=0; col<cols; col++) {
                    int splice_ind = -1;
                    if (Env::get_rand()/double(INT_MAX) < cross_over_chance) {
                        splice_ind = (Env::get_rand() % rows);
                    }
                    if (Env::get_rand() % 2) { // a first, then b
                        for (int row=0; row<=splice_ind; row++) {
                            temp_weights[
                                r * network.get_weights_ct() + 
                                weights_so_far + 
                                row * cols + 
                                col
                            ] = a_weights[
                                weights_so_far + 
                                row * cols + 
                                col
                            ];
                        }
                        for (int row=splice_ind+1; row<rows; row++) {
                            temp_weights[
                                r * network.get_weights_ct() + 
                                weights_so_far + 
                                row * cols + 
                                col
                            ] = b_weights[
                                weights_so_far + 
                                row * cols + 
                                col
                            ];
                        }
                    }
                    else { // b first, then a

                        for (int row=0; row<=splice_ind; row++) {
                            temp_weights[
                                r * network.get_weights_ct() + 
                                weights_so_far + 
                                row * cols + 
                                col
                            ] = b_weights[
                                weights_so_far + 
                                row * cols + 
                                col
                            ];
                        }
                        for (int row=splice_ind+1; row<rows; row++) {
                            temp_weights[
                                r * network.get_weights_ct() + 
                                weights_so_far + 
                                row * cols + 
                                col
                            ] = a_weights[
                                weights_so_far + 
                                row * cols + 
                                col
                            ];
                        }
                    }
                }
                weights_so_far += rows * cols;
                biases_so_far += cols;
            }
        }
        std::swap(weights, temp_weights);
        std::swap(biases, temp_biases);
        delete[] temp_weights;
        delete[] temp_biases;
    }


    void do_mutations(float mutation_chance) {
        const auto& layers = network.get_layer_sizes();
        for (int r=0; r<Env::num_rocks; r++) { // for each rocket
            int weights_so_far = 0;
            int biases_so_far = 0;
            for (int l=0; l<layers.size()-1; l++) { // for each layer 
                for (int row=0; row<layers[l]; row++) { // for each row
                    for (int col=0; col<layers[l+1]; col++) { // for each column
                        if (float(Env::get_rand())/INT_MAX < mutation_chance) {
                            weights[
                                (r * network.get_weights_ct()) + // weights per rocket
                                (weights_so_far) + // weights from previous layers
                                (row * layers[l+1]) + // weights from previous rows
                                (col) // current weight on row, col
                            ] += (Env::get_rand()%2 ? 1 : -1) * double(Env::get_rand())/(INT_MAX);
                        }
                    }
                }
                for (int col=0; col<layers[l+1]; col++) {
                    if (float(Env::get_rand())/INT_MAX < mutation_chance) {
                        biases[
                            (r * network.get_biases_ct()) + // biases per rocket
                            (biases_so_far) + // biases from previous layers
                            (col) // current col of bias
                        ] += (Env::get_rand()%2 ? 1 : -1) * double(Env::get_rand())/(INT_MAX);
                    }
                }
                weights_so_far += layers[l] * layers[l + 1];
                biases_so_far += layers[l + 1];
            } 
        }
    }

private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;

    // all inputs are stored as vectors next to each other
    // if inputs are [(1, 2), (4, 5), (3, 2)], rocket_inputs will be
    // [1, 2, 4, 5, 3, 2]
    // SIZE: number of rockets * number of inputs
    float* rocket_inputs;

    // all outputs are stored as vectors next to each other
    // if outputs are (1, 2), (4, 5), (3, 2), rocket_outputs will be
    // [1, 2, 4, 5, 3, 2]
    // SIZE: number of rockets * number of outputs
    float* rocket_outputs;

    // for each rocket: 
    // for each layer:
    // matrixes are stored in row major order, with each
    // matrix for every layer sotred next to each other, and 
    // with complete weights for each layer for each rocket
    // stored next to each other
    // SIZE: number of rockets * number of weights in all layers
    float* weights;

    // for each rocket:
    // for each layer:
    // each bias vector for each layer for each rocket is stored next
    // to each other
    // SIZE: number of rockets * number of biases in all layers
    float* biases;
};