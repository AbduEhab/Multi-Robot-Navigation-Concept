#include "Animation.h"

Animation::Animation()
{
}

Animation::Animation(size_t index, size_t frames, size_t speed)
{
    this->index = index;
    this->frames = frames;
    this->speed = speed;
}