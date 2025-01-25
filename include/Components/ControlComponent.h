#pragma once

#include "SpriteComponent.h"
#include "TransformComponent.h"
#include <Constants.hpp>
#include <EntityManager.h>
#include <Game.h>
#include <KeyMap.hpp>
#include <SDL.h>

class ControlComponent : public Component
{
public:
    SDL_KeyCode up_key;
    SDL_KeyCode down_key;
    SDL_KeyCode left_key;
    SDL_KeyCode right_key;

    TransformComponent *transform;
    SpriteComponent *sprite;

#ifdef DEBUG
    std::string name = "ControlComponent";
#endif // DEBUG

    ControlComponent() = default;
    ControlComponent(std::string up_key, std::string down_key, std::string left_key, std::string right_key)
    {
        this->up_key = get_sdl_key_code(up_key);
        this->down_key = get_sdl_key_code(down_key);
        this->left_key = get_sdl_key_code(left_key);
        this->right_key = get_sdl_key_code(right_key);
    }

    auto get_sdl_key_code(std::string &key) -> SDL_KeyCode
    {
        std::transform(key.begin(), key.end(), key.begin(), ::toupper);
        if (key_map.find(key) != key_map.end())
        {
            return key_map.at(key);
        }

        return SDLK_UNKNOWN;
    }

    auto init() -> void override
    {
        transform = owner->get_component<TransformComponent>();
        sprite = owner->get_component<SpriteComponent>();
    }

    auto update(const float delta_time) -> void override
    {
        if (Game::key_state == KEYSTATE::PRESSED)
        {
            SDL_Keycode key = Game::last_key;
            if (key == up_key)
            {
                transform->velocity = glm::vec2(0, -10);
                sprite->play("UpAnimation");
                return;
            }
            if (key == down_key)
            {
                transform->velocity = glm::vec2(0, 10);
                sprite->play("DownAnimation");
                return;
            }
            if (key == left_key)
            {
                transform->velocity = glm::vec2(-10, 0);
                sprite->play("LeftAnimation");
                return;
            }
            if (key == right_key)
            {
                transform->velocity = glm::vec2(10, 0);
                sprite->play("RightAnimation");
                return;
            }
        }
        if (Game::key_state == KEYSTATE::RELEASED)
        {
            SDL_Keycode key = Game::last_key;

            if (key == up_key || key == down_key || key == left_key || key == right_key)
            {
                transform->velocity = glm::vec2(0);
            }
        }
    }

    void debug_render()
    {
#ifdef DEBUG
        ImGui::Text("ControlComponent: ");
        ImGui::Text("Up Key: %s", SDL_GetKeyName(up_key));
        ImGui::Text("Down Key: %s", SDL_GetKeyName(down_key));
        ImGui::Text("Left Key: %s", SDL_GetKeyName(left_key));
        ImGui::Text("Right Key: %s", SDL_GetKeyName(right_key));

#endif // DEBUG
    }
};