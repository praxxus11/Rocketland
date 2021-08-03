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

    }

    ~AIManager() 
    {

    }
    void init_random(std::vector<Rocket>& rockets) {
        for (int i=0; i<rockets.size(); i++) {
            networks.emplace_back(&rockets[i], network.get_random_weights_biases());
        rockets[i].reset_rocket();
        }
    }

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


    void update_rockets() {
        if (all_landed) return;
        int all_done = 0;
        for (RocketManager& rm : networks) {
            rm.update_rocket(network);
            all_done += (rm.is_crashed() || rm.is_landed());
        }

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
            all_landed = 1;
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

            // if (Env::cycle_num%150==0) {
            //     std::cout << "\n\nSaving...\n\n";
            //     std::ofstream fout("C:/Users/Eric/ProgrammingProjectsCpp/RocketSaves/cycle_num" + std::to_string(Env::cycle_num) + ".txt");
            //     for (int i=0; i<Env::num_rocks/10; i++) {
            //         auto& hi = networks[i].get_wb();
            //         for (const auto& eigen_mat : hi) {
            //             fout << eigen_mat << '\n';
            //         }
            //         fout << '\n';
            //     }
            // }


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
                // rm.reset();
            }

        }
    }

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


    const std::vector<int>& get_layer_sizes() const {	
        return network.get_layer_sizes();	
    }

    std::vector<Eigen::MatrixXf>& get_wb() {
        return networks[0].get_wb();
    }
    const std::vector<std::vector<float>>& get_lbl_activations() const { // lbl = layer by layer
        return networks[0].get_lbl_activations();
    }
private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;
    bool all_landed = 0;
};