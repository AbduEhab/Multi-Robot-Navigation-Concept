#pragma once

#include "../EntityManager.h"
#include "../Game.h"
#include "imgui.h"
#include <SDL.h>
#include <glm/glm.hpp>

class NavigationGridElement
{
public:
    bool start = false;
    bool walkable = false;
    bool goal = false;
    bool visited = false;

    SDL_Texture *texture;

    NavigationGridElement()
    {
        set_texture("wall");
    }

    void set_texture(std::string texture_id)
    {
        texture = Game::asset_manager->get_texture(texture_id);

        if (!texture)
        {
            debug_print("Error: Texture is null, defaulting to error.png");
            texture = Game::asset_manager->get_texture("error");
        }
    }

    void update()
    {
    }

    void render(int x, int y, int scale)
    {
        TextureManager::draw(texture, {0, 0, 32, 32}, {x, y, scale, scale}, SDL_FLIP_NONE);
    }

    void data_render(bool update = true)
    {
        if (!update)
        {
            ImGui::BeginDisabled();
        }
        ImGui::Checkbox("Walkable", &walkable);
        ImGui::Checkbox("Goal", &goal);

        if (!update)
        {
            ImGui::EndDisabled();
        }

        ImGui::Text("Visited: %s", visited ? "true" : "false");

        ImGui::Text("Texture: %s", Game::asset_manager->get_texture_name(texture).c_str());

        if (start)
        {
            walkable = true;
            goal = false;
            // visited = false;

            texture = Game::asset_manager->get_texture("start");
        }
        if (!walkable)
        {
            start = false;
            goal = false;
            // visited = false;

            texture = Game::asset_manager->get_texture("wall");
        }
        if (goal)
        {
            start = false;
            walkable = true;
            // visited = false;

            texture = Game::asset_manager->get_texture("goal");
        }
    }
};

class NavigationGridComponent : public Component
{
public:
    bool initialized = false;
    size_t element_size;
    bool editable = false;

    std::unique_ptr<NavigationGridElement[]> grid;

    glm::vec2 position{0};

    size_t width;
    size_t height;

#ifdef DEBUG
    std::string name = "NavigationGridComponent";
#endif // DEBUG

    NavigationGridComponent(const size_t x, const size_t y, const size_t width, const size_t height, const size_t element_size)
    {
        this->position = glm::vec2(x, y);

        if (element_size == 0)
        {
            this->element_size = 32;
        }
        else
        {
            this->element_size = element_size;
        }

        this->width = width / element_size;
        this->height = height / element_size;

        init();
        initialized = true;
    }

    void init()
    {
        grid = std::make_unique<NavigationGridElement[]>(width * height);
    }

    void set_element_size(const size_t element_size)
    {
        if (element_size == 0)
            return;
        this->element_size = element_size;
        this->width = 920 / element_size;
        this->height = 660 / element_size;
        init();
    }

    void update(const float delta_time)
    {
    }

    void render()
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                grid[y * width + x].render(position.x + x * element_size, position.y + y * element_size, element_size);
            }
        }
    }

    void debug_render()
    {
#ifdef DEBUG
        ImGui::Text("initialized: %s", initialized ? "true" : "false");

        ImGui::DragFloat2("Position", &position.x, 10.f);

        ImGui::Checkbox("Prep Mode", &editable);
        if (editable)
            Game::game_state = GAMESTATE::SELECT;

        ImGui::InputInt("Element Size", (int *)&element_size);
#endif // DEBUG
    }

    std::string to_string() const
    {
        return std::string("Component<NavigationGridComponent>: (") + std::to_string(element_size) + std::string(")");
    }
};