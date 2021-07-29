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
        network(std::vector<int>{14, 12, 8}, 
        std::vector<NeuralNetwork::ActivationFuncs>{
            NeuralNetwork::ActivationFuncs::tanh,
            NeuralNetwork::ActivationFuncs::tanh
        })
    {   
#if defined(GPU)
        const auto& layers = network.get_layer_sizes();	

        rocket_inputs = new float[layers[0] * Env::num_rocks];	
        rocket_outputs = new float[layers[layers.size()-1] * Env::num_rocks];	

        weights = new float[network.get_weights_ct() * Env::num_rocks];	
        biases = new float[network.get_biases_ct() * Env::num_rocks];
#endif
    }

    ~AIManager() 
    {
#if defined(GPU) 
        delete[] rocket_inputs;	
        delete[] rocket_outputs;	
        delete[] weights;	
        delete[] biases;
#endif
    }
    void init_random(std::vector<Rocket>& rockets) {
        for (int i=0; i<rockets.size(); i++) {
#if defined(CPU)
            networks.emplace_back(&rockets[i], network.get_random_weights_biases());
#elif defined(GPU)
            networks.emplace_back(&rockets[i], i);
#endif    
        rockets[i].reset_rocket();
        }
#if defined(GPU)
        network.fill_random(weights, network.get_weights_ct() * Env::num_rocks);	
        network.fill_random(biases, network.get_biases_ct() * Env::num_rocks);
#endif
    }

#if defined(CPU)
    void init_from_file(std::vector<Rocket>& rockets, std::string filename) {
        std::vector<std::vector<Eigen::MatrixXf>> wts = network.get_wb_fromfile(filename);
        for (int i=0; i<rockets.size(); i++) {
            rockets[i].reset_rocket();
            networks.emplace_back(
                &rockets[i],
                wts[i % wts.size()]
            );
        }
    }
#elif defined(GPU)
    void init_from_file(std::vector<Rocket>& rockets, std::string filename) {
        for (int i=0; i<rockets.size(); i++) {	
            rockets[i].reset_rocket();
            networks.emplace_back(&rockets[i], i);	
        }	
        network.fill_from_file(weights, biases, filename);	
    }
#endif

    void update_rockets() {
        int all_done = 0;
#if defined(CPU)
        for (RocketManager& rm : networks) {
            rm.update_rocket(network);
            all_done += (rm.is_crashed() || rm.is_landed());
        }
#elif defined(GPU)
        const auto& layers = network.get_layer_sizes();	
        for (int i=0; i<networks.size(); i++) {	
            networks[i].update_inputs(&rocket_inputs[i * layers[0]]);	
        }	
        network.front_prop(rocket_inputs, rocket_outputs, weights, biases);	
        for (int i=0; i<networks.size(); i++) {	
            networks[i].update_outputs(&rocket_outputs[i * layers[layers.size() - 1]]);	
            all_done += (networks[i].is_crashed() || networks[i].is_landed());	
        }
#endif
        // Env::tempTm += Env::g_elapsed_real();
        // if (Env::tempTm>10) {
        //     Env::tempTm = 0;
        //     std::cout << all_done << '\n';
        //     for (int i=0; i<networks.size(); i++) {	
        //         if (!(networks[i].is_crashed() || networks[i].is_landed())) {
        //             networks[i].print_engine_stats();
        //         }
        //     }
        // }
        if (all_done >= Env::num_rocks) {
            Env::cycle_num++;
            double tot = 0;
            int ct = 0;
            for (RocketManager& rm : networks) {
                rm.updateScore();
                if (rm.getScore() < 5e6) {
                    tot += rm.getScore();
                    ct++;
                    if (rm.getScore() < -2000) {
                        std::cout << rm.getScore() << '\n';
                    }
                }
            }
            std::cout << "Iteration: " << Env::cycle_num << " Average score: " << tot/ct << "\n";
            
            sort(networks.begin(), networks.end(), [](const RocketManager& a, const RocketManager& b) {
                return (a.getScore() < b.getScore());
            });

#if defined(CPU)
            if (Env::cycle_num%150==0) {
                std::cout << "\n\nSaving...\n\n";
                std::ofstream fout("C:/Users/Eric/ProgrammingProjectsCpp/RocketSaves/cycle_num" + std::to_string(Env::cycle_num) + ".txt");
                for (int i=0; i<Env::num_rocks/10; i++) {
                    auto& hi = networks[i].get_wb();
                    for (const auto& eigen_mat : hi) {
                        fout << eigen_mat << '\n';
                    }
                    fout << '\n';
                }
            }
#elif defined(GPU)
            if (Env::cycle_num%150==0) {	
                std::cout << "\n\nSaving...\n\n";	
                std::ofstream fout("../saves/iteration" + std::to_string(Env::cycle_num) + ".txt");	
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
#endif

#if defined(CPU)
            std::vector<RocketManager> temp_rcks;
            for (RocketManager& rm : networks) {
                int a = round(Env::get_grad_rand());
                int b = round(Env::get_grad_rand());
                temp_rcks.emplace_back(
                    rm.get_rocket(),
                    do_cross_over(networks[a], networks[b])
                );
            }
            std::swap(temp_rcks, networks);
            for (RocketManager& rm : networks) {
                do_mutations(rm);
                rm.reset();
            }
#elif defined(GPU)
            do_cross_over(0.4, networks);	
            do_mutations(0.04*exp(-0.004*Env::cycle_num) + 0.006);
            for (int i=0; i<networks.size(); i++) {	
                networks[i].reset();	
                networks[i].set_index(i);	
            }
#endif
        }
    }

#if defined(CPU)
    std::vector<Eigen::MatrixXf> do_cross_over(RocketManager& a, RocketManager& b) {
        const float cross_over_chance = 0.4;
        std::vector<Eigen::MatrixXf> res;
        for (int layer=0; layer<a.get_wb().size(); layer++) {
            const int num_rows = a.get_wb()[layer].rows();
            const int num_cols = a.get_wb()[layer].cols();
            Eigen::MatrixXf temp(num_rows, num_cols);
            for (int col=0; col<num_cols; col++) {
                if (Env::get_rand()/double(INT_MAX) < cross_over_chance) {
                    int splice_ind = (Env::get_rand()%num_rows); // from [0, splice_ind], [splice_ind+1, num_rows-1]
                    if (Env::get_rand()%2) {
                        for (int i=0; i<=splice_ind; i++) {
                            temp(i, col) = a.get_wb()[layer](i, col);
                        }
                        for (int i=splice_ind+1; i<num_rows; i++) {
                            temp(i, col) = b.get_wb()[layer](i, col);
                        }
                    }
                    else {
                        for (int i=0; i<=splice_ind; i++) {
                            temp(i, col) = b.get_wb()[layer](i, col);
                        }
                        for (int i=splice_ind+1; i<num_rows; i++) {
                            temp(i, col) = a.get_wb()[layer](i, col);
                        }
                    }
                }
                else {
                    if (Env::get_rand()/double(INT_MAX) < 0.5) {
                        for (int i=0; i<num_rows; i++) {
                            temp(i, col) = a.get_wb()[layer](i, col);
                        }
                    }
                    else {
                        for (int i=0; i<num_rows; i++) {
                            temp(i, col) = b.get_wb()[layer](i, col);
                        }
                    }
                }
            }
            res.push_back(temp);
        }
        return res;
    }

#elif defined(GPU)
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
#endif

#if defined(CPU)
    void do_mutations(RocketManager& rm) {
        const float mutation_chance = 0.04*exp(-0.001*Env::cycle_num) + 0.006;
        for (Eigen::MatrixXf& mat : rm.get_wb()) {
            for (int i=0; i<mat.rows(); i++) {
                for (int j=0; j<mat.cols(); j++) {
                    if (float(Env::get_rand())/INT_MAX < mutation_chance)
                        mat(i, j) += (Env::get_rand()%2 ? 1 : -1) * double(Env::get_rand())/(INT_MAX);
                }
            }
        }
    }

#elif defined(GPU)	
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
#endif

private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;
#if defined(GPU)
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
#endif
};