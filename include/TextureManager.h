#pragma once

#include "Game.h"

class TextureManager
{
public:
    static SDL_Texture *load_texture(const char* texture_file); 
    static void draw(SDL_Texture* texture, SDL_Rect source_rect, SDL_Rect destination_rect, SDL_RendererFlip flip);
};
