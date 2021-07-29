#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <Eigen/Dense>
#include <algorithm>

class RocketManager {
public:
    RocketManager(
        Rocket* rocket, 
#if defined(CPU)
        const std::vector<Eigen::MatrixXf>& wb
#elif defined(GPU)
        int ind
#endif
        ) :
        rocket_ref(rocket),
        score(0),
#if defined(CPU)
        weights_biases(wb)
#elif defined(GPU)
        index(ind)
#endif
    {
    }

#if defined(CPU)
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
                                p.angle/180.f, float(tanh(0.005 * p.angle_vel)),
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

        //heuristic
        for (int i=0; i<3; i++) {
            if (rocket_ref->is_engine_on(i)) {
                engine_used[i]++;
            }
        }
    }
#elif defined(GPU)
      void update_inputs(float* inp) {	
        if (rocket_ref->irlGetPosition().y > 1500 || abs(rocket_ref->irlGetPosition().x) > 1100) {	
            rocket_ref->setStatus(Rocket::Status::BlewUp);	
            score = 1e7;	
        }	
        if (rocket_ref->getStatus() != Rocket::Status::Regular) return;	
        StateParams p = rocket_ref->get_rocket_params();	
        float angle = p.angle;	
        if (angle > 180) angle -= 360;	
        inp[0] = tanh(0.001 * p.posy);	
        inp[1] = tanh(0.01 * p.posx);	
        inp[2] = tanh(0.01 * p.vely);	
        inp[3] = tanh(0.01 * p.velx);	
        inp[4] = p.angle / 180.f;	
        inp[5] = tanh(0.005 * p.angle_vel);	
        inp[6] = p.e1_thr;	
        inp[7] = p.e1_angle / 15.f;	
        inp[8] = p.e2_thr;	
        inp[9] = p.e2_angle / 15.f;	
        inp[10] = p.e3_thr;	
        inp[11] = p.e3_angle / 15.f;	
        inp[12] = p.uflp_angle / 90.f;	
        inp[13] = p.lflp_angle / 90.f;	
    }	
    	
    void update_outputs(float* oup) {	
        rocket_ref->update_params(ControlParams(	
            oup[0], oup[1],	
            oup[2], oup[3],	
            oup[4], oup[5],	
            oup[6], oup[7]	
        ));	
        for (int i=0; i<3; i++) {
            if (rocket_ref->is_engine_on(i)) {
                engine_used[i]++;
            }
        }
    }
#endif

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

        // heuristics
        // want all engines to be equally on
        std::sort(engine_used.begin(), engine_used.end());
        float diff_norm = float(engine_used[2] - engine_used[0]) / engine_used[2];
        score += (5 * (pow(60.f, diff_norm) - 1));

        if (!is_crashed() && !is_landed()) 
            score = INT_MAX;
        if (is_landed()) 
            score -= 2000;
    }

    int getScore() const { return score; }
    
    void reset() {
        score = 0;
        rocket_ref->reset_rocket();
        for (int& ct : engine_used) ct = 0;
    }

#if defined(CPU)
    std::vector<Eigen::MatrixXf>& get_wb() {
        return weights_biases;
    }
    Rocket* get_rocket() {
        return rocket_ref;
    }
#endif

#if defined(GPU)
    int get_index() const {
        return index;
    }
    void set_index(int i) {
        index = i;
    }
#endif

private:
    Rocket* rocket_ref;
    int score;
    std::array<int, 3> engine_used = {0, 0, 0};
#if defined(CPU)
    std::vector<Eigen::MatrixXf> weights_biases;
#elif defined(GPU)
    int index;
#endif
};