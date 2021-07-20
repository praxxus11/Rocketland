#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <Eigen/Dense>

class RocketManager {
public:
    RocketManager(Rocket* rocket, 
        const std::vector<Eigen::MatrixXf>& wb) :
        weights_biases(wb),
        rocket_ref(rocket)
    {
    }
    void update_rocket(const NeuralNetwork& nn) {
        if (rocket_ref->getStatus() != Rocket::Status::Regular) return;
        StateParams p = rocket_ref->get_rocket_params();
        std::vector<float> inp {float(tanh(0.001 * p.posy)), float(tanh(0.01 * p.posx)),
                                float(tanh(0.01 * p.vely)), float(tanh(0.01 * p.velx)),
                                p.angle/360.f, float(tanh(0.005 * p.angle_vel)),
                                p.e1_thr, p.e1_angle/15.f,
                                p.e2_thr, p.e2_angle/15.f,
                                p.e3_thr, p.e3_angle/15.f,
                                p.uflp_angle/90.f, p.lflp_angle/90.f};
        std::vector<float> res = nn.front_prop(inp, weights_biases);
        rocket_ref->update_params(ControlParams(
            res[0], res[1],
            res[2], res[3],
            res[4], res[5],
            res[6], res[7]
        ));
    }
private:
    Rocket* rocket_ref;
    std::vector<Eigen::MatrixXf> weights_biases;
};