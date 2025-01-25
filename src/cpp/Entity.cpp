#include "Entity.h"

Entity::Entity(EntityManager &manager) : manager(manager)
{
    active = true;
}

Entity::Entity(EntityManager &manager, std::string name) : manager(manager), name(name)
{
    active = true;
}

void Entity::update(float deltaTime)
{
    for (auto &component : components)
    {
        component->update(deltaTime);
    }
}

void Entity::render() const
{
    for (auto &component : components)
    {
        component->render();
    }
}

void Entity::destroy()
{
    active = false;
}

bool Entity::is_active() const
{
    return active;
}

void Entity::list_all_components(std::string indentation) const
{
    for (auto &component : components_by_types)
    {
        std::cout << std::string(indentation) + std::string("Component<") + std::string(component.first->name()).substr(2) + ">" << std::endl;
    }
}

std::string Entity::to_string()
{
    std::string s;

    s = s.append("  ") + name + ":\n{";

    for (auto &component : components)
    {
        s = s.append("\t") + component->to_string() + "\n";
    }

    return s + "}";
}