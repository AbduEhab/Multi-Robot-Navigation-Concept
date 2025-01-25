#pragma once

#include "../EntityManager.h"
#include "../Game.h"
#include "imgui.h"
#include <SDL.h>
#include <glm/glm.hpp>

class TransformComponent : public Component
{
public:
    glm::vec2 position{0};
    glm::vec2 velocity{0};

    int width = 10;
    int height = 10;
    float scale = 10;

#ifdef DEBUG
    std::string name = "TransformComponent";
#endif // DEBUG

    TransformComponent(const int pos_X, const int pos_Y, const int vel_X, const int vel_Y, const int width, const int height, const float scale)
        : position(glm::vec2(pos_X, pos_Y)), velocity(glm::vec2(vel_X, vel_Y)),
          width(width), height(height), scale(scale) {}

    void update(const float delta_time)
    {
        position.x += velocity.x * delta_time;
        position.y += velocity.y * delta_time;
    }

    void debug_render()
    {
#ifdef DEBUG
        float pos2[2] = {position.x, position.y};
        ImGui::DragFloat2("Position", pos2, 10.f);
        position = glm::vec2(pos2[0], pos2[1]);

        ImGui::BeginDisabled();
        float vel2[2] = {velocity.x, velocity.y};
        ImGui::DragFloat2("Velocity", vel2, 10.f);
        velocity = glm::vec2(vel2[0], vel2[1]);
        ImGui::EndDisabled();

        ImGui::DragInt("Width", &width, 1, 0, INT_MAX);
        ImGui::DragInt("Height", &height, 1, 0, INT_MAX);
        ImGui::DragFloat("Scale", &scale, 0.5, 0.01, 10);
#endif // DEBUG
    }

    std::string to_string() const
    {
        return std::string("Component<TransformComponent>: (") + std::to_string(position.x) + std::string(", ") + std::to_string(position.y) + std::string(")");
    }
};