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

void AssetManager::add_texture(std::string texture_id, const char *texture_file)
{
    textures.emplace(texture_id, TextureManager::load_texture(texture_file));
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
    return "Texture not found";
}