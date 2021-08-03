#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <Eigen/Dense>


struct NestedCircle : public sf::Drawable, public sf::Transformable {
    NestedCircle(int inner_rad, int outer_rad) {
        inner_circle.setRadius(inner_rad);
        outer_circle.setRadius(outer_rad);
        inner_circle.setOrigin(sf::Vector2f(inner_rad, inner_rad));
        outer_circle.setOrigin(sf::Vector2f(outer_rad, outer_rad));

        // change
        outer_circle.setFillColor(sf::Color::Black);
        inner_circle.setFillColor(sf::Color::White);
    }
    void set_outer_circle_color(sf::Color color) {
        outer_circle.setFillColor(color);
    }
private:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        states.transform *= getTransform();
        target.draw(outer_circle, states);
        target.draw(inner_circle, states);
    }
    sf::CircleShape inner_circle;
    sf::CircleShape outer_circle;
};

class NNDrawer : public sf::Drawable {
public:
    NNDrawer(const sf::Vector2f& pos, const std::vector<int>& l_sizes) :
        layer_sizes(l_sizes),
        connections_to_rckt(l_sizes[l_sizes.size()-1], sf::VertexArray(sf::Lines, 2))
    {
        const int inner_circle_rad = 7;
        const int out_circle_rad = 15;
        const int space_layer = 60;
        const int space_nodes = 35;
    
        for (int i=0; i<layer_sizes.size(); i++) {
            std::vector<NestedCircle> column;
            const int diff = ((l_sizes[0] - l_sizes[i]) * space_nodes) / 2;
            for (int j=0; j<layer_sizes[i]; j++) {
                NestedCircle tempc(inner_circle_rad, out_circle_rad);
                tempc.setPosition(sf::Vector2f(pos.x + i*space_layer, pos.y + j*space_nodes + diff));
                column.push_back(tempc);
            }
            nodes.push_back(column);
        }
        for (int i=0; i<layer_sizes.size()-1; i++) {
            std::vector<std::vector<sf::VertexArray>> layer;
            for (int j=0; j<layer_sizes[i]; j++) {
                std::vector<sf::VertexArray> column;
                for (int k=0; k<layer_sizes[i+1]; k++) {
                    sf::VertexArray line(sf::Lines, 2);
                    line[0].position = nodes[i][j].getPosition();
                    line[1].position = nodes[i+1][k].getPosition();
                    line[0].color = sf::Color(0, 0, 0, 100);
                    line[1].color = sf::Color(0, 0, 0, 100);
                    column.push_back(line);
                }
                layer.push_back(column);
            }
            connections.push_back(layer);
        }
    }   

    void update(const std::vector<Eigen::MatrixXf>& wb, // to didplay lines between nodes
    const std::vector<std::vector<float>>& lbl_activations, // to display node colors
    const Rocket& rocket_ref) { // to display lines from output to rocket itself
        for (int i=0; i<layer_sizes.size()-1; i++) {
            for (int j=0; j<layer_sizes[i]; j++) {
                for (int k=0; k<layer_sizes[i+1]; k++) {
                    auto& connection = connections[i][j][k];
                    const float weightval = wb[i](j, k);
                    const int red  = int(255 * (1 / (1 + exp(-0.2 * weightval))));
                    const int green  = int(255 * (1 / (1 - exp(-0.2 * weightval))));
                    connection[0].color = sf::Color(red, green, 0, 100);
                    connection[1].color = sf::Color(red, green, 0, 100);
                }
            }
        }
        for (int i=0; i<connections_to_rckt.size(); i++) {
            connections_to_rckt[i][0].position = nodes[nodes.size()-1][i].getPosition();
            
            const float activation_val = lbl_activations[lbl_activations.size()-1][i];
            const int red  = int(255 * (1 / (1 + exp(-2 * activation_val))));
            const int green  = int(255 * (1 / (1 - exp(-2 * activation_val))));
            connections_to_rckt[i][0].color = sf::Color(red, green, 0, 200);
        }

        connections_to_rckt[0][1].position = rocket_ref.get_engine_position(0); // engine number 1
        connections_to_rckt[1][1].position = rocket_ref.get_engine_position(0); // engine number 1
        connections_to_rckt[2][1].position = rocket_ref.get_engine_position(1); // engine number 2
        connections_to_rckt[3][1].position = rocket_ref.get_engine_position(1); // engine number 2
        connections_to_rckt[4][1].position = rocket_ref.get_engine_position(2); // engine number 3
        connections_to_rckt[5][1].position = rocket_ref.get_engine_position(2); // engine number 1
        connections_to_rckt[6][1].position = rocket_ref.get_flap_position(RocketFins::Type::Upper); // upper flap angle
        connections_to_rckt[7][1].position = rocket_ref.get_flap_position(RocketFins::Type::Lower); // lower flap angle
        
        for (int i=0; i<nodes.size(); i++) {
            for (int j=0; j<nodes[i].size(); j++) {
                const float activation_val = lbl_activations[i][j];
                const int red  = int(255 * (1 / (1 + exp(-2 * activation_val))));
                const int green  = int(255 * (1 / (1 - exp(-2 * activation_val))));
                nodes[i][j].set_outer_circle_color(sf::Color(red, green, 0, 200));
            }
        }
    }
private:
    // struct NestedCircle;
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override {
        for (const auto& layer : connections) {
            for (const auto& column : layer) {
                for (const auto& connection : column) {
                    target.draw(connection);
                }
            }
        }

        for (const auto& connection : connections_to_rckt) {
            target.draw(connection);
        }

        for (const auto& column : nodes) {
            for (const auto& node : column) {
                target.draw(node);
            }
        }
    }
    std::vector<int> layer_sizes;
    std::vector<std::vector<NestedCircle>> nodes;
    std::vector<std::vector<std::vector<sf::VertexArray>>> connections;
    std::vector<sf::VertexArray> connections_to_rckt;
};
