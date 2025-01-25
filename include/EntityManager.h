#pragma once

#include "Component.h"
#include "Entity.h"

class Entity;

class EntityManager
{
private:
    std::vector<Entity *> entities;

public:
    void update(float delta_time);
    void render() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] bool is_empty() const;
    [[nodiscard]] Entity &add_entity(std::string entity_name);
    [[nodiscard]] std::vector<Entity *> get_entities() const;
    void list_all_entities() const;
    bool clear();
};