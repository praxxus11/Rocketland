#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <fstream>

#include "NeuralNetwork.h"
#include "RocketManager.h"

class AIManager {
public:
    AIManager() : 
        network(std::vector<int>{14, 14, 8}, 
        std::vector<NeuralNetwork::ActivationFuncs>{
            NeuralNetwork::ActivationFuncs::tanh,
            // NeuralNetwork::ActivationFuncs::tanh,
            // NeuralNetwork::ActivationFuncs::tanh,
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
        totTime += Env::g_elapsed();
        if (all_done) {
            Env::cycle_num++;
            totTime = 0;
            double tot = 0;
            int ct = 0;
            for (RocketManager& rm : networks) {
                rm.updateScore();
                if (rm.getScore() < 5e6) {
                    tot += rm.getScore();
                    ct++;
                }
            }
            sort(networks.begin(), networks.end(), [](const RocketManager& a, const RocketManager& b) {
                return (a.getScore() < b.getScore());
            });
            std::cout << "Iteration: " << iter++ << " Average score: " << tot/ct << "\n";
            
            // first remove the worst 90% of them, meanwhile also crosses best of them
            const int top_x = Env::num_rocks/10;
            for (int i=Env::num_rocks-1; i>top_x; i--) {
                int a = int(rand()%top_x), b = int(rand()%top_x);
                if (a==b) b++;
                do_cross_over(networks[a], networks[b]);
                networks[i].get_wb() = networks[rand()%2 ? a : b].get_wb();
            }

            // now cross best again
            for (int i=0; i<top_x; i++) {
                int a = int(rand()%top_x), b = int(rand()%top_x);
                if (a==b) b++;
                do_cross_over(networks[a], networks[b]);
            }

            for (RocketManager& rm : networks) {
                do_mutations(rm);
                rm.reset();
            }
            if (Env::cycle_num%50==0) {
                std::cout << "\n\nSaving...\n\n";
                std::ofstream fout("saves/save_cycle" + std::to_string(Env::cycle_num) + ".txt");
                for (int i=0; i<30; i++) {
                    auto& hi = networks[i].get_wb();
                    for (const auto& eigen_mat : hi) {
                        fout << eigen_mat << '\n';
                    }
                    fout << '\n';
                }
            }
        }
    }

    void do_cross_over(RocketManager& a, RocketManager& b) {
        for (int layer=0; layer<a.get_wb().size(); layer++) {
            int num_rows = a.get_wb()[layer].rows();
            int num_cols = a.get_wb()[layer].cols();
            for (int row=0; row<num_rows; row++) {
                int splice_ind = (rand()%num_cols); // from [0, splice_ind], [splice_ind+1, num_rows-1]
                if (rand()%2) {
                    for (int i=0; i<=splice_ind; i++) {
                        float temp = a.get_wb()[layer](row, i);
                        a.get_wb()[layer](row, i) = b.get_wb()[layer](row, i);
                        b.get_wb()[layer](row, i) = temp;
                    }
                }
                else {
                    for (int i=splice_ind+1; i<num_cols; i++) {
                        float temp = a.get_wb()[layer](row, i);
                        a.get_wb()[layer](row, i) = b.get_wb()[layer](row, i);
                        b.get_wb()[layer](row, i) = temp;
                    }
                }
            }
        }
    }

    void do_mutations(RocketManager& rm) {
        for (Eigen::MatrixXf& mat : rm.get_wb()) {
            for (int i=0; i<mat.rows(); i++) {
                for (int j=0; j<mat.cols(); j++) {
                    if (rand()/RAND_MAX < 0.01)
                        mat(i, j) += (rand()%2 ? 1 : -1) * rand()/(3*RAND_MAX);
                }
            }
        }
    }

private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;
    int iter = 0;
    float totTime = 0;
};