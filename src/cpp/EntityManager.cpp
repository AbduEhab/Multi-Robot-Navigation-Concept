#include "EntityManager.h"

void EntityManager::render() const
{
    for (auto &entity : entities)
    {
        entity->render();
    }
}

void EntityManager::update(float delta_time)
{
    for (auto &entity : entities)
    {
        entity->update(delta_time);
    }
}

bool EntityManager::clear()
{
    for (auto &entity : entities)
    {
        entity->destroy();
    }

    return true;
}

size_t EntityManager::size() const
{
    return entities.size();
}

Entity &EntityManager::add_entity(std::string entity_name)
{
    Entity *entity = new Entity(*this, entity_name);

    entities.emplace_back(entity);

    return *entity;
}

bool EntityManager::is_empty() const
{
    return entities.size() == 0;
}

std::vector<Entity *> EntityManager::get_entities() const
{
    return entities;
}

void EntityManager::list_all_entities() const
{

    std::cout << "EntityManager<EntityDump>:\n{" << std::endl;

    for (auto entity : entities)
    {
        std::cout << "\t" << entity->name << "\n\t{" << std::endl;
        entity->list_all_components("\t\t");

        std::cout << "\t}" << std::endl;
    }

    std::cout << "}" << std::endl;
}