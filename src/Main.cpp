#define SDL_MAIN_HANDLED

#include "Engine.h"
#include "Game.h"

int main(int argc, char *arg[])
{

    Game game = Game();

    game.init(1400, 900);

    Engine::end_time_of_last_frame = Clock::now();

    while (game.is_running())
    {
        Engine::calculate_deltatime_and_wait();

        game.process_input();
        game.update(Engine::delta_time);
        game.render(Engine::delta_time);
    }

    game.destroy();

    return 0;
}