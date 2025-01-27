#pragma once

#include "../EntityManager.h"
#include "../Game.h"
#include "imgui.h"
#include <SDL.h>
#include <glm/glm.hpp>

#include "json.hpp"

class NavigationGridElement
{
public:
    bool start = false;
    bool walkable = false;
    bool goal = false;
    bool visited = false;

    size_t x, y;

    size_t g_cost = 0; // distance from start
    size_t h_cost = 0; // distance from goal
    size_t f_cost = 0; // g_cost + h_cost

    SDL_Texture *texture;

    NavigationGridElement()
    {
        set_texture("wall");
    }

    void set_coord(size_t x, size_t y)
    {
        this->x = x;
        this->y = y;

        start = false;
        walkable = false;
        goal = false;
        visited = false;

        g_cost = 0;
        h_cost = 0;
        f_cost = 0;

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

    void update(int x, int y, int scale)
    {
               // void update(glm::vec2 offset, int scale)
        //  {
        //      int x = offset.x + x * scale;
        //      int y = offset.y + y * scale;
        glm::vec2 mouse_pos = Game::mouse_selection[1];

        SDL_Rect rect = {x, y, scale, scale};

        // glm to sdl point
        SDL_Point point = {(int)mouse_pos.x, (int)mouse_pos.y};

        if (SDL_PointInRect(&point, &rect))
        {
            switch (Game::game_state)
            {
            case GAMESTATE::SET_START:
                start = true;
                walkable = true;
                goal = false;
                visited = false;

                texture = Game::asset_manager->get_texture("start");
                break;

            case GAMESTATE::SET_GOAL:
                start = false;
                walkable = true;
                goal = true;
                visited = false;

                texture = Game::asset_manager->get_texture("goal");
                break;

            case GAMESTATE::SET_WALKABLE:
                start = false;
                walkable = true;
                goal = false;
                visited = false;

                texture = Game::asset_manager->get_texture("walkable");
                break;

            case GAMESTATE::SET_WALL:
                start = false;
                walkable = false;
                goal = false;
                visited = false;

                texture = Game::asset_manager->get_texture("wall");
                break;
            }
        }
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

    // serialize to nlohmann json
    nlohmann::json to_json() const
    {
        nlohmann::json j;

        j["start"] = start;
        j["walkable"] = walkable;
        j["goal"] = goal;
        j["visited"] = visited;

        j["texture"] = Game::asset_manager->get_texture_name(texture);

        return j;
    }

    void from_json(const nlohmann::json &j)
    {
        start = j["start"];
        walkable = j["walkable"];
        goal = j["goal"];
        visited = j["visited"];

        set_texture(j["texture"]);
    }
};

class NavigationGridComponent : public Component
{
public:
    size_t element_size;
    bool editable = false;

    std::unique_ptr<NavigationGridElement[]> grid;

    glm::vec2 position{0};

    size_t width;
    size_t height;

    size_t ref_width;
    size_t ref_height;

#ifdef DEBUG
    std::string name = "NavigationGridComponent";
#endif // DEBUG

    NavigationGridComponent(const size_t x, const size_t y, const size_t width, const size_t height, size_t element_size)
    {
        this->position = glm::vec2(x, y);

        ref_width = width;
        ref_height = height;

        if (element_size == 0)
        {
            debug_print("Error: Element size cannot be 0, setting it to 32");
            this->element_size = 32;
            return;
        }
        else
            this->element_size = element_size;
    }

    void init()
    {
        this->width = ref_width / element_size;
        this->height = ref_height / element_size;

        grid = std::make_unique<NavigationGridElement[]>(width * height);
        
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                grid[y * width + x].set_coord(x, y);
            }
        }
    }

    void set_element_size(const size_t element_size)
    {
        if (element_size == 0)
            return;

        this->element_size = element_size;

        init();
    }

    void update(const float delta_time)
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                grid[y * width + x].update(position.x + x * element_size, position.y + y * element_size, element_size);
            }
        }
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
        ImGui::DragFloat2("Position", &position.x, 10.f);

        ImGui::SeparatorText("Setup");

        if (ImGui::Button("Toggle Edit Mode"))
        {
            editable = !editable;
        }

        if (editable)
        {
            switch (Game::game_state)
            {
            case GAMESTATE::PREPARE:
                if (ImGui::Button("Resize"))
                    Game::game_state = GAMESTATE::RESIZE;
                if (ImGui::Button("Set Start"))
                    Game::game_state = GAMESTATE::SET_START;
                if (ImGui::Button("Set Goal"))
                    Game::game_state = GAMESTATE::SET_GOAL;
                if (ImGui::Button("Set Walkable"))
                    Game::game_state = GAMESTATE::SET_WALKABLE;
                if (ImGui::Button("Set Wall"))
                    Game::game_state = GAMESTATE::SET_WALL;
                break;

            case GAMESTATE::RESIZE:
                ImGui::Text("Drag to resize");
                if (ImGui::Button("Save"))
                    Game::game_state = GAMESTATE::PREPARE;
                break;

            case GAMESTATE::SET_START:
                ImGui::Text("Set Start by selecting start area");
                if (ImGui::Button("Save"))
                    Game::game_state = GAMESTATE::PREPARE;
                break;

            case GAMESTATE::SET_GOAL:
                ImGui::Text("Set Goal by selecting goal area (can be multiple)");
                if (ImGui::Button("Save"))
                    Game::game_state = GAMESTATE::PREPARE;
                break;

            case GAMESTATE::SET_WALKABLE:
                ImGui::Text("Set Walkable by selecting walkable area");
                if (ImGui::Button("Save"))
                    Game::game_state = GAMESTATE::PREPARE;
                break;

            case GAMESTATE::SET_WALL:
                ImGui::Text("Set Walls by selecting them");
                if (ImGui::Button("Save"))
                    Game::game_state = GAMESTATE::PREPARE;
                break;
            }
        }

        ImGui::SeparatorText("");

        if (ImGui::Button("Reset"))
        {
            set_element_size(32);
            editable = false;
            Game::game_state = GAMESTATE::PREPARE;
        }

        const auto es = element_size;
        ImGui::InputInt("Element Size", (int *)&element_size);

        if (es != element_size)
        {
            set_element_size(element_size);
        }
#endif // DEBUG
    }

    void save(const std::string &file_name)
    {
        debug_print("Saving Navigation Grid to: " + file_name);
        nlohmann::json j;

        j["position"] = {position.x, position.y};
        j["element_size"] = element_size;
        j["width"] = width;
        j["height"] = height;

        nlohmann::json grid_data;

        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                grid_data.push_back(grid[y * width + x].to_json());
            }
        }

        j["grid"] = grid_data;

        std::ofstream file(file_name);
        file << j.dump(4);
        file.close();
    }

    void load(const std::string &file_name)
    {
        debug_print("Loading Navigation Grid from: " + file_name);
        std::ifstream file(file_name);

        if (!file.is_open())
        {
            debug_print("Error: File not found: " + file_name);
            return;
        }

        nlohmann::json j;
        file >> j;
        file.close();

        position.x = j["position"][0];
        position.y = j["position"][1];

        element_size = j["element_size"];
        width = j["width"];
        height = j["height"];

        init();

        nlohmann::json grid_data = j["grid"];

        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                grid[y * width + x].from_json(grid_data[y * width + x]);
            }
        }
    }

    std::string to_string() const
    {
        return std::string("Component<NavigationGridComponent>: (") + std::to_string(element_size) + std::string(")");
    }
};