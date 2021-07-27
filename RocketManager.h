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
        if (rocket_ref->irlGetPosition().y > 1500 || abs(rocket_ref->irlGetPosition().x) > 1100) {
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
                                p.e1_thr, p.e1_angle/15.f,
                                p.e2_thr, p.e2_angle/15.f,
                                p.e2_thr, p.e2_angle/15.f,
                                p.uflp_angle/90.f, p.lflp_angle/90.f};

        std::vector<float> res = nn.front_prop(inp, weights_biases);
        rocket_ref->update_params(ControlParams(
            res[0], res[1],
            res[2], res[3],
            res[4], res[5],
            res[6], res[7]
        ));
    }

    bool is_crashed() const {
        return (rocket_ref->getStatus() == Rocket::Status::BlewUp);
    }
    bool is_landed() const {
        return (rocket_ref->getStatus() == Rocket::Status::Landed);
    }

    void updateScore() {
        // cost from going too fast downards
        const float vel_y_cost = abs(rocket_ref->getVelocity().y) * 30;

        // cost from going too fast sideways 
        const float vel_x_cost = abs(rocket_ref->getVelocity().x) * 20;

        const float angle_vel_cost = abs(rocket_ref->get_angular_vel()) * 23;
        
        const float angle_cost = 100 * (-abs(180 - rocket_ref->getRotation()) + 180);   
        
        const float temp_tot_score = vel_y_cost + vel_x_cost + angle_vel_cost + angle_cost;
        const float distr = 1;

        score += (distr * (vel_y_cost/temp_tot_score)) * vel_y_cost;
        score += (distr * (vel_x_cost/temp_tot_score)) * vel_x_cost;
        score += (distr * (angle_vel_cost/temp_tot_score)) * angle_vel_cost;
        score += (distr * (angle_cost/temp_tot_score)) * angle_cost;

        if (!is_crashed() && !is_landed()) {
            score = INT_MAX;
        }
        if (is_landed()) 
            score -= 2000;
    }
    int getScore() const { return score; }
    
    void reset() {
        score = 0;
        rocket_ref->reset_rocket();
    }

    Rocket* get_rocket() {
        return rocket_ref;
    }

    std::vector<Eigen::MatrixXf>& get_wb() {
        return weights_biases;
    }

private:
    Rocket* rocket_ref;
    std::vector<Eigen::MatrixXf> weights_biases;
    int score;
};