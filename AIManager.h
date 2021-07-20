#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#include "NeuralNetwork.h"
#include "RocketManager.h"

class AIManager {
public:
    AIManager() : 
        network(std::vector<int>{14, 8, 8}, 
        std::vector<NeuralNetwork::ActivationFuncs>{
            NeuralNetwork::ActivationFuncs::tanh,
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
            for (RocketManager& rm : networks) {
                rm.updateScore();
                tot += rm.getScore();
            }
            sort(networks.begin(), networks.end(), [](const RocketManager& a, const RocketManager& b) {
                return (a.getScore() < b.getScore());
            });
            std::cout << "Iteration: " << iter++ << " Best score: " << tot/100 << "\n";
            
            // first remove the worst 90% of them, meanwhile also crosses best of them
            const int top_x = 10;
            for (int i=99; i>top_x; i--) {
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
        }
    }

    void do_cross_over(RocketManager& a, RocketManager& b) {
        for (int layer=0; layer<a.get_wb().size(); layer++) {
            int num_rows = a.get_wb()[layer].rows();
            int num_cols = a.get_wb()[layer].cols();
            for (int col=0; col<num_cols; col++) {
                int splice_ind = (rand()%num_rows); // from [0, splice_ind], [splice_ind+1, num_rows-1]
                if (rand()%2) {
                    for (int i=0; i<=splice_ind; i++) {
                        float temp = a.get_wb()[layer](i, col);
                        a.get_wb()[layer](i, col) = b.get_wb()[layer](i, col);
                        b.get_wb()[layer](i, col) = temp;
                    }
                }
                else {
                    for (int i=splice_ind+1; i<num_rows; i++) {
                        float temp = a.get_wb()[layer](i, col);
                        a.get_wb()[layer](i, col) = b.get_wb()[layer](i, col);
                        b.get_wb()[layer](i, col) = temp;
                    }
                }
            }
        }
    }

    void do_mutations(RocketManager& rm) {
        const float rand_chance = rand()/7;
        for (Eigen::MatrixXf& mat : rm.get_wb()) {
            for (int i=0; i<mat.rows(); i++) {
                for (int j=0; j<mat.cols(); j++) {
                    if (rand()/RAND_MAX < rand_chance)
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