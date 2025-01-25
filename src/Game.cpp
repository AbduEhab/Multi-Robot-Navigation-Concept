#include <iostream>

#include <Components/ControlComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/TransformComponent.h>
#include <Game.h>

EntityManager manager;
AssetManager *Game::asset_manager = new AssetManager(&manager);
SDL_Renderer *Game::renderer;

KEYSTATE Game::key_state;

SDL_Keycode Game::last_key;
MOUSESTATE Game::mouse_state;

glm::uvec2 Game::mouse_selection[2]{{0, 0}, {0, 0}};

[[nodiscard]] bool Game::is_running() const
{
    return running;
}

void Game::init(int width, int height) // init SDL
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Error init sdl." << std::endl;
        return;
    }
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE);
    if (!window)
    {
        std::cerr << "error with window." << std::endl;
        return;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        std::cerr << "error with renderer" << std::endl;
        return;
    }
    SDL_SetWindowTitle(window, "Multi-Robot-Navigation-Concept");

    SDL_GL_SetSwapInterval(VSYNC); // enable vsync

    // init imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    load_level(0);

    running = true;
}

void Game::load_level([[maybe_unused]] int level_number) const
{
    asset_manager->clear();

    asset_manager->add_texture("map", "../assets/final-slam-map.png");
    // asset_manager->add_texture("target", "../assets/collision-texture.png");

    switch (level_number)
    {
    case 0:
        Entity &base_map = manager.add_entity("base-map");
        (void)base_map.add_component<TransformComponent>(330, 80, 0, 0, 920, 660, 1);
        (void)base_map.add_component<SpriteComponent>("map");
        (void)base_map.add_component<ControlComponent>("w", "s", "a", "d");

        // Entity &target = manager.add_entity("test_target");
        // (void)target.add_component<TransformComponent>(50, 50, 0, 0, 32, 32, 1);
        // (void)target.add_component<SpriteComponent>("target");

        break;
    }
}

void Game::process_input()
{
    SDL_Event event;
    bool kb_state_updated = false;
    bool ms_state_updated = false;
    // poll for events
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);

        switch (event.type) // get event type and switch on it
        {
        case SDL_QUIT:       // escape key on the window
            running = false; // break game loop
            break;

        case SDL_MOUSEWHEEL:
            if (event.wheel.y > 0) // scroll up
            {
                scale += 0.1f;
            }
            else if (event.wheel.y < 0) // scroll down
            {
                scale -= 0.1f;
            }
            break;

        // if the middle mouse button is pressed reset the scale
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_MIDDLE)
            {
                scale = 1.0f;
            }
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                if (mouse_state == MOUSESTATE::RELEASED)
                {
                    mouse_selection[0] = {event.button.x, event.button.y};
                    mouse_selection[1] = {event.button.x, event.button.y};
                    mouse_state = MOUSESTATE::PRESSED;
                    ms_state_updated = true;
                }
            }
            break;

        case SDL_MOUSEMOTION:
            if ((mouse_state == MOUSESTATE::PRESSED || mouse_state == MOUSESTATE::DRAGGING) && event.motion.state & SDL_BUTTON_LMASK)
            {
                mouse_selection[1] = {event.motion.x, event.motion.y};
                mouse_state = MOUSESTATE::DRAGGING;
                ms_state_updated = true;
            }
            break;

            // case SDL_MOUSEBUTTONDOWN:
            //     if (!leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT)
            //     {
            //         leftMouseButtonDown = true;

            //         for (auto rect : rectangles)
            //         {
            //             if (SDL_PointInRect(&mousePos, rect))
            //             {
            //                 selectedRect = rect;
            //                 break;
            //             }
            //         }
            //     }

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                mouse_selection[1] = {event.button.x, event.button.y};
                mouse_state = MOUSESTATE::RELEASED;
                ms_state_updated = true;
            }
            break;

        case SDL_KEYDOWN: // if any key is pressed down

            if (!kb_state_updated)
            {
                key_state = KEYSTATE::PRESSED;
                last_key = event.key.keysym.sym;
                kb_state_updated = true;
            }

            if (event.key.keysym.sym == SDLK_ESCAPE) // if this key is the Esc key
            {
                print("Escape key pressed");
                running = false;
            }
            else if (event.key.keysym.sym == SDLK_q)
            {
                print("Q key pressed, quitting...");
                running = false;
            }
            break;

        case SDL_KEYUP: // if any key is released

            if (!kb_state_updated)
            {
                key_state = KEYSTATE::RELEASED;
                kb_state_updated = true;
            }
            break;

        default:
            if (!kb_state_updated)
                key_state = KEYSTATE::NONE;
            // if (!ms_state_updated)
            //     mouse_state = MOUSESTATE::NONE;
            break;
        }
    }
}

TimePoint time_s = Clock::now();

void Game::update(const float delta_time)
{
    manager.update(delta_time);

    bool res = std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - time_s).count() >= FRAME_TIME_TARGET;

    if (res) [[unlikely]]
    {
        time_s = Clock::now();
    }
}

void Game::render(const float delta_time)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Entity Manager");

    for (auto &entity : manager.get_entities())
    {
        if (ImGui::TreeNode(entity->name.c_str()))
        {
            for (auto &component : entity->components)
            {
                auto name = std::string(typeid(*component).name()).erase(0, 2);

                ImGui::SeparatorText(name.c_str());
                component->debug_render();
            }
            ImGui::TreePop();
        }
    }
    ImGui::End();

    ImGui::Begin("Game Data");
    ImGui::Text("FPS: %1f", 1.0f / delta_time);
    ImGui::Text("Frametime: %f", delta_time);
    ImGui::Text("RunTime: %u", SDL_GetTicks64());
    ImGui::Text("Last Button Pressed: %s", SDL_GetKeyName(last_key));

    auto get_mouse_selection = [&]() -> std::string
    {
        return "(" + std::to_string(mouse_selection[0].x) + "," + std::to_string(mouse_selection[0].y) + "), (" + std::to_string(mouse_selection[1].x) + "," + std::to_string(mouse_selection[1].y) + ")";
    };
    ImGui::Text("Mouse Selection: %s", get_mouse_selection().c_str());
    ImGui::Text("Mouse Selection Distance: %f", glm::distance(glm::vec2(mouse_selection[0]), glm::vec2(mouse_selection[1])));
    ImGui::End();

    SDL_RenderSetScale(renderer, scale, scale); // set the scale of the renderer

    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255); // set up the given renderer to render a specific color
    SDL_RenderClear(renderer);                         // clear back buffer with the specified color

    manager.render();

    // draw a line in red
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawLine(renderer, mouse_selection[0].x, mouse_selection[0].y, mouse_selection[1].x, mouse_selection[1].y);
    

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer); // swap back and front buffer
}

void Game::destroy()
{
    // imgui
    print("Destroying ImGui Context");
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // sdl
    print("Clearing Asset Manager");
    asset_manager->clear();

    print("Destroying SDL Renderer");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}