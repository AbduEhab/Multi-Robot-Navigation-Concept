#pragma once

#include "../EntityManager.h"
#include "../Game.h"
#include "imgui.h"
#include <SDL.h>
#include <glm/glm.hpp>

#include "json.hpp"

class NavigationGridElement;

class NavigationGridElement
{
public:
    bool start = false;
    bool walkable = false;
    bool goal = false;
    bool visited = false;

    bool occupied = false;

    int path_assigned_to = 0;

    bool path = false;
    NavigationGridElement *parent = nullptr;

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

    void update(int x, int y, int scale, int path_index)
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

                path_assigned_to = path_index;

                texture = Game::asset_manager->get_texture("start");
                break;

            case GAMESTATE::SET_GOAL:
                start = false;
                walkable = true;
                goal = true;
                visited = false;

                path_assigned_to = path_index;

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

        SDL_Rect rect = {x, y, scale, scale};
        SDL_Point mouse_pos_sdl = {(int)Game::mouse_pos.x, (int)Game::mouse_pos.y};
        if (SDL_PointInRect(&mouse_pos_sdl, &rect))
        {
            TextureManager::draw(Game::asset_manager->get_texture("target"), {0, 0, 32, 32}, {x, y, scale, scale}, SDL_FLIP_NONE);
            ImGui::Begin("Element Data");
            ImGui::Text("Position: (%d, %d)", x, y);

            ImGui::Text("G: %d", g_cost);
            ImGui::Text("H: %d", h_cost);
            ImGui::Text("F: %d", f_cost);

            ImGui::Text("Type: %s", start ? "Start" : goal   ? "Goal"
                                                  : walkable ? "Walkable"
                                                             : "Wall");
            ImGui::Text("Path Assigned To: %d", (path_assigned_to + 1));
            ImGui::End();
        }
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
        if (path)
        {
            start = false;
            goal = false;

            texture = Game::asset_manager->get_texture("path0");
        }
        if (visited && (!start || !goal) && !path)
        {
            start = false;
            goal = false;

            texture = Game::asset_manager->get_texture("walked0");
        }
        if (occupied)
        {
            if (path_assigned_to == 0)
            {
                texture = Game::asset_manager->get_texture("p1");
            }
            else
            {
                texture = Game::asset_manager->get_texture("p2");
            }
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
        j["path"] = path;
        j["occupied"] = occupied;
        j["path_assigned_to"] = path_assigned_to;

        j["texture"] = Game::asset_manager->get_texture_name(texture);

        return j;
    }

    void from_json(const nlohmann::json &j)
    {
        start = j["start"];
        walkable = j["walkable"];
        goal = j["goal"];
        visited = j["visited"];
        path = j["path"];
        occupied = j["occupied"];
        path_assigned_to = j["path_assigned_to"];

        set_texture(j["texture"]);
    }
};

class NavigationGridElementCompare
{
public:
    bool operator()(NavigationGridElement *a, NavigationGridElement *b)
    {
        return a->f_cost >= b->f_cost;
    }
};

enum class PATHACTION
{
    WALK,
    STOP,
};

class PathContainer
{
public:
    NavigationGridElement *start = nullptr;
    NavigationGridElement *goal = nullptr;
    std::vector<NavigationGridElement *> path;
    bool goal_found = false;

    std::vector<PATHACTION> actions;

    uint32_t verification_index = 0;
    bool verification_complete = false;

    nlohmann::json get_path_json()
    {
        nlohmann::json j;

        for (auto element : path)
        {
            j.push_back({element->x, element->y});
        }

        return j;
    }

    nlohmann::json get_actions_json()
    {
        nlohmann::json j;

        for (auto &action : actions)
        {
            j.push_back((int)action);
        }

        return j;
    }
};

class NavigationGridComponent : public Component
{
public:
    size_t element_size;

    bool editable = false;
    bool path_find_running = false;
    bool verification_running = false;
    bool verification_successful = false;

    int path_index = 0;
    std::vector<PathContainer> paths; // bool is for if a goal was found

    std::priority_queue<NavigationGridElement *, std::vector<NavigationGridElement *>, NavigationGridElementCompare> open_set;

    std::unique_ptr<NavigationGridElement[]> grid;

    glm::vec2 *position;

    size_t width;
    size_t height;

    size_t ref_width;
    size_t ref_height;

    const float *scale;

#ifdef DEBUG
    std::string name = "NavigationGridComponent";
#endif // DEBUG

    NavigationGridComponent(glm::vec2 *position, const size_t width, const size_t height, size_t element_size, const float *scale)
    {
        this->position = position;

        this->scale = scale;

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

        paths.clear();
        paths.push_back(PathContainer());
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
                grid[y * width + x].update(position->x + x * element_size * *scale, position->y + y * element_size * *scale, element_size * *scale, path_index);
            }
        }
    }

    void render()
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                grid[y * width + x].render(position->x + x * element_size * *scale, position->y + y * element_size * *scale, element_size * *scale);
            }
        }
    }

    void prepare_path_find(int index)
    {
        if (index >= paths.size())
        {
            debug_print("Error: Index out of bounds");
            return;
        }

        path_find_running = false;

        open_set = std::priority_queue<NavigationGridElement *, std::vector<NavigationGridElement *>, NavigationGridElementCompare>();

        auto &path = paths[index];
        path.start = nullptr;
        path.goal = nullptr;
        path.goal_found = false;
        path.path.clear();

        // get start and goal points
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                auto &element = grid[y * width + x];

                if (element.start && (element.path_assigned_to == index))
                {
                    if (path.start)
                    {
                        debug_print("Error: Multiple Start Points Defined for Path: " + std::to_string(index));
                        return;
                    }
                    path.start = &element;
                }

                if (element.goal && (element.path_assigned_to == index))
                {
                    if (path.goal)
                    {
                        debug_print("Error: Multiple Goal Points Defined for Path: " + std::to_string(index));
                        return;
                    }
                    path.goal = &element;
                }
            }
        }

        if (!path.start || !path.goal)
        {
            debug_print("Error: Start or Goal not set");
            return;
        }

        if (path.start == path.goal)
        {
            debug_print("Error: Start and Goal are the same");
            return;
        }

        path.start->visited = true;

        // get 4 neighbours
        NavigationGridElement *neighbours[4] = {
            path.start->x > 0 ? &grid[path.start->y * width + path.start->x - 1] : nullptr,
            path.start->y > 0 ? &grid[(path.start->y - 1) * width + path.start->x] : nullptr,
            path.start->x < width - 1 ? &grid[path.start->y * width + path.start->x + 1] : nullptr,
            path.start->y < height - 1 ? &grid[(path.start->y + 1) * width + path.start->x] : nullptr};

        for (auto neighbour : neighbours)
        {
            if (!neighbour || !neighbour->walkable || neighbour->visited)
                continue;

            size_t new_g_cost = path.start->g_cost + 1;
            size_t new_h_cost = glm::abs((int)neighbour->x - (int)path.goal->x) + glm::abs((int)neighbour->y - (int)path.goal->y);
            size_t new_f_cost = new_g_cost + new_h_cost;

            if (new_f_cost < neighbour->f_cost || !neighbour->parent)
            {
                neighbour->g_cost = new_g_cost;
                neighbour->h_cost = new_h_cost;
                neighbour->f_cost = new_f_cost;
                neighbour->parent = path.start;

                open_set.push(neighbour);
            }
        }

        path_find_running = true;
    }

    void reset_nodes()
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                auto &element = grid[y * width + x];

                element.visited = false;
                element.g_cost = 0;
                element.h_cost = 0;
                element.f_cost = 0;
                element.parent = nullptr;
            }
        }
    }

    bool step(int curr_path)
    {
        auto current = open_set.top();
        open_set.pop();

        auto start = paths[curr_path].start;
        auto goal = paths[curr_path].goal;

        if ((current == goal) && (curr_path == current->path_assigned_to))
        {
            debug_print("Path Found for index: " + std::to_string(curr_path));

            // assign path
            paths[curr_path].path.push_back(current);
            for (auto current = goal->parent; current && (current != start); current = current->parent)
            {
                paths[curr_path].path.push_back(current);

                current->path = true;
                current->data_render(false);
            }
            // reverse path
            std::reverse(paths[curr_path].path.begin(), paths[curr_path].path.end());

            paths[curr_path].goal_found = true;

            return true;
        }

        current->visited = true;
        current->data_render(false);

        // get 4 neighbours
        NavigationGridElement *neighbours[4] = {
            current->x > 0 ? &grid[current->y * width + current->x - 1] : nullptr,
            current->y > 0 ? &grid[(current->y - 1) * width + current->x] : nullptr,
            current->x < width - 1 ? &grid[current->y * width + current->x + 1] : nullptr,
            current->y < height - 1 ? &grid[(current->y + 1) * width + current->x] : nullptr};

        for (auto cell : neighbours)
        {
            if (!cell || !cell->walkable || cell->visited)
                continue;

            size_t new_g_cost = current->g_cost + 1;
            size_t new_h_cost = glm::abs((int)cell->x - (int)goal->x) + glm::abs((int)cell->y - (int)goal->y);
            size_t new_f_cost = new_g_cost + new_h_cost;

            if (new_f_cost < cell->f_cost || !cell->parent)
            {
                cell->g_cost = new_g_cost;
                cell->h_cost = new_h_cost;
                cell->f_cost = new_f_cost;
                cell->parent = current;

                open_set.push(cell);
            }
        }

        return false;
    }

    void step_verification()
    {
        for (auto i = 0; i < paths.size(); i++)
        {
            auto &path = paths[i];

            if (path.verification_complete)
                continue;

            if (path.verification_index >= path.path.size())
            {
                path.verification_complete = true;
                continue;
            }

            auto current = path.path[path.verification_index];
            if (current->occupied)
            {
                debug_print("Path is bloacked for index: " + std::to_string(i),
                            " at location: (" + std::to_string(current->x) + ", " + std::to_string(current->y) + ")", ". Waiting for it to clear");
                path.actions.push_back(PATHACTION::STOP);
                continue;
            }

            path.actions.push_back(PATHACTION::WALK);
            current->occupied = true;
            current->path_assigned_to = i;
            current->data_render(false);

            auto prev = path.verification_index > 0 ? path.path[path.verification_index - 1] : nullptr;

            if (prev)
            {
                prev->occupied = false;
            }

            path.verification_index++;

            if (path.verification_index >= path.path.size())
            {
                path.verification_complete = true;
            }
        }
    }

    void debug_render()
    {
        // #ifdef DEBUG
        ImGui::SeparatorText("Setup");

        if (ImGui::Button("Toggle Edit Mode"))
        {
            editable = !editable;
        }

        if (editable)
        {
            ImGui::Text("Number of paths: %d", paths.size());
            if (ImGui::Button("Add Path"))
            {
                paths.push_back(PathContainer());
            }
            ImGui::SameLine();
            if (ImGui::Button("Remove Path"))
            {
                // remove the element at the current path_index
                if (paths.size() > 1)
                    paths.erase(paths.begin() + path_index);
            }

            path_index = path_index + 1;
            ImGui::InputInt("Path to Edit", &path_index);
            path_index = path_index - 1;

            if (path_index >= paths.size())
            {
                path_index = paths.size() - 1;
            }
            if (path_index < 0)
            {
                path_index = 0;
            }

            path_find_running = false;
            verification_running = false;

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
            path_find_running = false;
            verification_running = false;
            Game::game_state = GAMESTATE::PREPARE;
        }

        const auto es = element_size;
        ImGui::InputInt("Element Size", (int *)&element_size);

        if (es != element_size)
        {
            set_element_size(element_size);
        }

        ImGui::SeparatorText("Path Finding");

        if (ImGui::Button("Enter Path Find Mode"))
        {
            editable = false;
            verification_running = false;
            reset_nodes();
            prepare_path_find(path_index);
        }

        if (path_find_running)
        {
            if (ImGui::Button("Step (only for debug purposes)"))
            {
                if (!open_set.empty())
                {
                    if (step(path_index))
                        path_find_running = false;
                }
                else
                    path_find_running = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Run to End"))
            {
                for (size_t i = 0; i < paths.size(); i++)
                {
                    reset_nodes();
                    prepare_path_find(i);

                    while (!open_set.empty())
                        if (step(i))
                            break;
                }
                path_find_running = false;
            }
        }

        ImGui::Text("Path Find Running: %s", path_find_running ? "true" : "false");
        for (size_t i = 0; i < paths.size(); i++)
        {
            ImGui::Text("Path %s for index %i", paths[i].goal_found ? "found" : "could not be found for path", i + 1);
        }

        ImGui::SeparatorText("Verification");

        if (ImGui::Button("Enter Verification Mode"))
        {
            editable = false;
            path_find_running = false;
            verification_running = true;

            for (size_t i = 0; i < paths.size(); i++)
            {
                if (!paths[i].goal_found)
                {
                    ImGui::Text("Error: Path %i not found. Please make sure paths were generated", i + 1);
                    return;
                }
                else
                {
                    paths[i].verification_index = 0;
                    paths[i].verification_complete = false;
                }
            }

            reset_nodes();
        }

        if (verification_running)
        {
            if (ImGui::Button("Step (only for debug purposes)"))
            {
                verification_running = false;
                for (auto &path : paths)
                {
                    if (!path.verification_complete)
                    {
                        verification_running = true;
                    }
                }
                if (!verification_running)
                {
                    verification_successful = true;
                }
                step_verification();
            }
        }

        if (verification_successful)
        {
            if (ImGui::Button("Export Paths"))
            {
                debug_print("Exporting Paths");

                nlohmann::json j;

                for (auto &path : paths)
                {
                    nlohmann::json path_data;
                    debug_print("Adding path to json");
                    path_data["path"] = path.get_path_json();
                    debug_print("Adding actions to json");
                    path_data["actions"] = path.get_actions_json();

                    path_data["element_size"] = element_size;

                    j.push_back(path_data);
                }
                
                std::ofstream file("paths.json");
                file << j.dump(4);
                file.close();
            }
        }

        // #endif // DEBUG
    }

    void save(const std::string &file_name)
    {
        debug_print("Saving Navigation Grid to: " + file_name);
        nlohmann::json j;

        j["position"] = {position->x, position->y};
        j["element_size"] = element_size;
        j["width"] = width;
        j["height"] = height;

        j["paths"] = paths.size();

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

        position->x = j["position"][0];
        position->y = j["position"][1];

        element_size = j["element_size"];
        width = j["width"];
        height = j["height"];

        init();

        paths = std::vector<PathContainer>(j["paths"]);

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