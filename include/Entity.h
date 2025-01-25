#pragma once

#include "Constants.hpp"

#include "Component.h"
#include "EntityManager.h"

#include <map>
#include <string>
#include <typeinfo>
#include <vector>

class Component;
class EntityManager;

class [[nodiscard]] Entity
{
public:
    std::string name;
    std::vector<Component *> components;

    Entity(EntityManager &manager);
    Entity(EntityManager &manager, std::string name);

    void list_all_components(std::string indentation = "") const;

    [[nodiscard]] bool is_active() const;

    void update(float delta_time);
    void render() const;
    
    void destroy();

    std::string to_string();

    template <typename T, typename... TArgs>
    [[nodiscard]] constexpr T &add_component(TArgs &&...args)
    {
        T *comp(new T(std::forward<TArgs>(args)...));
        comp->owner = this;
        comp->init();
        components.emplace_back(comp);
        components_by_types[&typeid(*comp)] = comp;
        return *comp;
    }

    template <typename T>
    [[nodiscard]] constexpr T *get_component()
    {
        return static_cast<T *>(components_by_types[&typeid(T)]);
    }

    template <typename T>
    [[nodiscard]] constexpr bool has_component()
    {
        return !(components_by_types[&typeid(T)] == nullptr);
    }

    private:
    EntityManager &manager;
    bool active;
    std::map<const std::type_info *, Component *> components_by_types;
};