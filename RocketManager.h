#include <SFML/Graphics.hpp>
#include <iostream>
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
        std::vector<float> inp {p.posy, p.posx,
                                p.vely, p.velx,
                                p.angle, p.angle_vel,
                                p.e1_thr, p.e1_angle,
                                p.e2_thr, p.e2_angle,
                                p.e3_thr, p.e3_angle,
                                p.uflp_angle, p.lflp_angle};
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