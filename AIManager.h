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
        network(std::vector<int>{8, 12, 2}, 
        std::vector<NeuralNetwork::ActivationFuncs>{
            // NeuralNetwork::ActivationFuncs::tanh,
            NeuralNetwork::ActivationFuncs::tanh,
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
        bool all_done = 1;
        for (RocketManager& rm : networks) {
            rm.update_rocket(network);
            all_done = all_done && (rm.is_crashed() || rm.is_landed());
        }

        if (all_done) {
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


            if (Env::cycle_num%100==0) {
                std::cout << "\n\nSaving...\n\n";
                std::ofstream fout("C:/Users/Eric/ProgrammingProjectsCpp/RocketSaves/cycle_num" + std::to_string(Env::cycle_num) + ".txt");
                for (int i=0; i<30; i++) {
                    auto& hi = networks[i].get_wb();
                    for (const auto& eigen_mat : hi) {
                        fout << eigen_mat << '\n';
                    }
                    fout << '\n';
                }
            }

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

            // first remove the worst 90% of them, meanwhile also crosses best of them
            // const int top_x = Env::num_rocks/10;

            // for (int i=Env::num_rocks-1; i>top_x; i--) {
                // int a = Env::get_rand()%top_x, b = Env::get_rand()%top_x;
                // networks[i].get_wb() = do_cross_over(networks[a], networks[b]);
            // }

            for (int i=Env::num_rocks-1; i>Env::num_rocks-50; i--) {
                switch(rand()%3) {
                    case 0:
                        networks[i].get_wb() = do_cross_over(networks[0], networks[1]);
                        break;
                   case 1:
                        networks[i].get_wb() = networks[0].get_wb();
                        break;
                   case 2:
                        networks[i].get_wb() = networks[1].get_wb();
                        break;
                }
            }

            for (RocketManager& rm : networks) {
                do_mutations(rm);
                rm.reset();
            }
        }
    }

    std::vector<Eigen::MatrixXf> do_cross_over(RocketManager& a, RocketManager& b) {
        std::vector<Eigen::MatrixXf> res;
        for (int layer=0; layer<a.get_wb().size(); layer++) {
            int num_rows = a.get_wb()[layer].rows();
            int num_cols = a.get_wb()[layer].cols();
            Eigen::MatrixXf temp(num_rows, num_cols);
            for (int row=0; row<num_rows; row++) {
                int splice_ind = (Env::get_rand()%num_cols); // from [0, splice_ind], [splice_ind+1, num_rows-1]
                if (Env::get_rand()%2) {
                    for (int i=0; i<=splice_ind; i++) {
                        temp(row, i) = a.get_wb()[layer](row, i);
                    }
                    for (int i=splice_ind+1; i<num_cols; i++) {
                        temp(row, i) = b.get_wb()[layer](row, i);
                    }
                }
                else {
                    for (int i=0; i<=splice_ind; i++) {
                        temp(row, i) = b.get_wb()[layer](row, i);
                    }
                    for (int i=splice_ind+1; i<num_cols; i++) {
                        temp(row, i) = a.get_wb()[layer](row, i);
                    }
                }
            }
            res.push_back(temp);
        }
        return res;
    }

    void do_mutations(RocketManager& rm) {
        const float mutation_chance = 0.05*exp(-0.01*Env::cycle_num) + 0.006;
        for (Eigen::MatrixXf& mat : rm.get_wb()) {
            for (int i=0; i<mat.rows(); i++) {
                for (int j=0; j<mat.cols(); j++) {
                    if (float(Env::get_rand())/INT_MAX < mutation_chance)
                        mat(i, j) += (Env::get_rand()%2 ? 1 : -1) * double(Env::get_rand())/(INT_MAX);
                }
            }
        }
    }

private:
    NeuralNetwork network;
    std::vector<RocketManager> networks;
};