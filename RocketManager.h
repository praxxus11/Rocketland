#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <Eigen/Dense>

class RocketManager {
public:
    RocketManager(Rocket* rocket, 
        const std::vector<Eigen::MatrixXf>& wb) :
        weights_biases(wb),
        rocket_ref(rocket),
        score(0)
    {
    }
    void update_rocket(const NeuralNetwork& nn) {
        if (rocket_ref->irlGetPosition().y > 1300 || abs(rocket_ref->irlGetPosition().x) > 1000) {
            rocket_ref->setStatus(Rocket::Status::BlewUp);
            score = 1e7;
        }
        if (rocket_ref->getStatus() != Rocket::Status::Regular) return;
        StateParams p = rocket_ref->get_rocket_params();
        float angle = p.angle;
        if (angle > 180) angle -= 360;
        std::vector<float> inp {float(tanh(0.001 * p.posy)), float(tanh(0.01 * p.posx)),
                                float(tanh(0.01 * p.vely)), float(tanh(0.01 * p.velx)),
                                p.angle/180, float(tanh(0.005 * p.angle_vel)),
                                p.e_thr, p.e_angle/15.f};

        std::vector<float> res = nn.front_prop(inp, weights_biases);
        rocket_ref->update_params(ControlParams(
            res[0], res[1]
        ));
    }

    bool is_crashed() const {
        return (rocket_ref->getStatus() == Rocket::Status::BlewUp);
    }
    bool is_landed() const {
        return (rocket_ref->getStatus() == Rocket::Status::Landed);
    }

    void updateScore() {
        score += abs(rocket_ref->getVelocity().y) * 30;
        score += abs(rocket_ref->getVelocity().x) * 15;
        score += abs(rocket_ref->get_angular_vel()) * 10;
        score += 100 * (-abs(180 - rocket_ref->getRotation()) + 180);   
        if (!is_crashed() && !is_landed()) {
            score = INT_MAX;
        }
    }
    int getScore() const { return score; }
    
    void reset() {
        score = 0;
        rocket_ref->reset_rocket();
    }

    std::vector<Eigen::MatrixXf>& get_wb() {
        return weights_biases;
    }

private:
    Rocket* rocket_ref;
    std::vector<Eigen::MatrixXf> weights_biases;
    int score;
};