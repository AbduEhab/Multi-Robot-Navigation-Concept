#pragma once

#include "Constants.hpp"

#include <SDL.h>
#include <SDL_image.h>

#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <imgui.h>

// #include <SDL_ttf.h>

#include "AssetManager.h"
#include "Component.h"
#include "Entity.h"
#include "EntityManager.h"

#include <glm/glm.hpp>

class AssetManager;
enum class KEYSTATE : int
{
    PRESSED,
    RELEASED,
    NONE
};

enum class MOUSESTATE : int
{
    PRESSED,
    RELEASED,
    DRAGGING,
};

enum class GAMESTATE : int
{
    PREPARE,
    SELECT,
    CALCULATE,
    QUIT
};

class Game
{
private:
    bool running = false;
    SDL_Window *window; // the sdl frame

public:
    Game() = default;

    static AssetManager *asset_manager;
    static KEYSTATE key_state;
    static SDL_Keycode last_key;

    static glm::uvec2 mouse_selection[2];
    static MOUSESTATE mouse_state;

    static GAMESTATE game_state;
    
    float scale = 1.f;

    [[nodiscard]] bool is_running() const;
    static SDL_Renderer *renderer; // the sdl graphics renderer
    void load_level(int level_number) const;
    void init(int width, int height);
    void process_input();
    void update(const float delta_time);
    void render(const float delta_time);
    void destroy();
};
