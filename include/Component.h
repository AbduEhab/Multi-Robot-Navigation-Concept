#pragma once

#include "Entity.h"

class Entity;

class Component
{
public:
    Entity *owner;
    virtual ~Component() {}
    virtual void init() {}
    virtual void update(float delta_time) = 0;
    virtual void render() {}
    virtual void debug_render() {}

#ifdef DEBUG
    std::string name;
#endif // DEBUG

    virtual std::string to_string() const
    {
        return std::string("Component Does not have a to_string method.");
    }
};