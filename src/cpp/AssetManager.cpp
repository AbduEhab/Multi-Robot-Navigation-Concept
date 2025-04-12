#include "AssetManager.h"

AssetManager::AssetManager(EntityManager *manager) : manager(manager)
{
}

void AssetManager::clear()
{
    for (auto &texture : textures)
    {
        SDL_DestroyTexture(texture.second);
    }
    textures.clear();
}

AssetManager::~AssetManager()
{
    print("Destroying Asset Manager");
    clear();
}

bool AssetManager::add_texture(std::string texture_id, const char *texture_file)
{
    if (textures.find(texture_id) != textures.end())
    {
        debug_print("Error: Texture ID already exists: " + texture_id);
        return false; // texture already exists
    }

    textures.emplace(texture_id, TextureManager::load_texture(texture_file));

    return true;
}

bool AssetManager::remove_texture(std::string texture_id)
{
    auto it = textures.find(texture_id);
    if (it != textures.end())
    {
        SDL_DestroyTexture(it->second);
        textures.erase(it);
        return true;
    }
    debug_print("Error: Texture ID not found, can't delete: " + texture_id);
    return false;
}

SDL_Texture *AssetManager::get_texture(std::string texture_id)
{
    return textures[texture_id];
}

std::string AssetManager::get_texture_name(SDL_Texture *texture)
{
    for (auto &texture_name : textures)
    {
        if (texture_name.second == texture)
        {
            return texture_name.first;
        }
    }
    return "Requested Texture not found";
}