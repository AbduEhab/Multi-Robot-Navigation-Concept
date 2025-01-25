#pragma once

#include "Constants.hpp"
#include "SDL.h"

namespace Engine
{
    // template <typename T>
    // using Ref = std::shared_ptr<T>;

    // template <typename T>
    // using Scope = std::unique_ptr<T>;

#define ticks_since_epoch() std::chrono::high_resolution_clock::now().time_since_epoch()

    inline static float delta_time;

    inline static TimePoint end_time_of_last_frame;

    inline float get_delta_time()
    {
        return (float)(Clock::now() - end_time_of_last_frame).count();
    }

    inline float get_delta_time_in_microseconds()
    {
        return (Clock::now() - end_time_of_last_frame).count() * 1e-3;
    }

    inline float get_delta_time_in_milliseconds()
    {
        return (Clock::now() - end_time_of_last_frame).count() * 1e-6;
    }

    inline float get_delta_time_in_seconds()
    {
        return (Clock::now() - end_time_of_last_frame).count() * 1e-9;
    }

    inline void calculate_deltatime_and_wait()
    {
        float waitTime = FRAME_TIME_TARGET - (Engine::get_delta_time_in_milliseconds()); // calculate time to wait between frames

        if constexpr (FPS > 0)
        {
            if (waitTime > 0 && waitTime <= FRAME_TIME_TARGET)
                [[unlikely]]
            {
                // SDL_Delay(waitTime);
                std::this_thread::sleep_for(std::chrono::milliseconds((int)waitTime)); // hold exec until the specified ms has passed
            }
        }

        delta_time = Engine::get_delta_time_in_seconds(); // diffrence between the time since the last frame and the current frame in seconds

        Engine::end_time_of_last_frame = Clock::now(); // update the lastFrameTime time to be used in the next pass
    }
}