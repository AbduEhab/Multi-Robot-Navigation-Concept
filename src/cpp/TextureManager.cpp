#include "TextureManager.h"

SDL_Texture *TextureManager::load_texture(const char *textureFile)
{
    SDL_Surface *surface = IMG_Load(textureFile);
    if (!surface)
    {
        debug_print("Error: ", IMG_GetError());
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(Game::renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void TextureManager::draw(SDL_Texture *texture, SDL_Rect sourceRect, SDL_Rect destinationRect, SDL_RendererFlip flip)
{
    SDL_RenderCopyEx(Game::renderer, texture, &sourceRect, &destinationRect, 0, 0, flip);
}