#pragma once

#include "EntityManager.h"
#include "TextureManager.h"
#include <map>

class AssetManager
{
private:
    EntityManager *manager;
    std::map<std::string, SDL_Texture *> textures;

public:
    AssetManager(EntityManager *manager);
    ~AssetManager();
    void clear();
    void add_texture(std::string texture_id, const char *asset_file);
    SDL_Texture *get_texture(std::string texture_id);
    std::string get_texture_name(SDL_Texture *texture);
};
