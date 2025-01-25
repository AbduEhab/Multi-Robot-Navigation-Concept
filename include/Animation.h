#pragma once

#include <Constants.hpp>

struct Animation
{
    size_t index;
    size_t frames;
    size_t speed;

    Animation();
    Animation(size_t index, size_t frames, size_t speed);
};