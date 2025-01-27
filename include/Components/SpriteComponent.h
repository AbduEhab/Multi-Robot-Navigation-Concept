#pragma once

#include "../AssetManager.h"
#include "../TextureManager.h"
#include "Animation.h"
#include "TransformComponent.h"

class SpriteComponent : public Component
{
public:
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    bool modulate = false;
    u_char mod[4] = {255, 255, 255, 255};

    bool is_visible = true;

#ifdef DEBUG
    std::string name = "SpriteComponent";
#endif // DEBUG

    SpriteComponent(std::string texture_id, bool is_fixed = false)
    {
        set_texture(texture_id);

        is_animated = false;
        this->is_fixed = is_fixed;
    }

    SpriteComponent(std::string texture_id, int frame_count, int animation_speed, bool hasDirections, bool is_fixed)
    {
        is_animated = true;
        this->frame_count = frame_count;
        this->animation_speed = animation_speed;
        this->is_fixed = is_fixed;

        if (hasDirections)
        {
            Animation downAnimation = Animation(0, frame_count, animation_speed);
            Animation rightAnimation = Animation(1, frame_count, animation_speed);
            Animation leftAnimation = Animation(2, frame_count, animation_speed);
            Animation upAnimation = Animation(3, frame_count, animation_speed);

            animations.emplace("DownAnimation", downAnimation);
            animations.emplace("RightAnimation", rightAnimation);
            animations.emplace("LeftAnimation", leftAnimation);
            animations.emplace("UpAnimation", upAnimation);

            this->animation_index = 0;
            this->current_animation_name = "DownAnimation";
        }
        else
        {
            Animation singleAnimation = Animation(0, frame_count, animation_speed);
            animations.emplace("SingleAnimation", singleAnimation);
            this->animation_index = 0;
            this->current_animation_name = "SingleAnimation";
        }

        play(this->current_animation_name);
        set_texture(texture_id);
    }

    void play(std::string animationName)
    {
        frame_count = animations[animationName].frames;
        animation_index = animations[animationName].index;
        animation_speed = animations[animationName].speed;
        current_animation_name = animationName;
    }

    void set_texture(std::string texture_id)
    {
        texture = Game::asset_manager->get_texture(texture_id);

        if (!texture)
        {
            debug_print("Error: Texture is null, defaulting to error.png");
            texture = Game::asset_manager->get_texture("error");
        }
    }

    void init()
    {
        transform = owner->get_component<TransformComponent>();
        source_rect.x = 0;
        source_rect.y = 0;
        source_rect.w = transform->width;
        source_rect.h = transform->height;

        destiation_rect.x = (int)transform->position.x;
        destiation_rect.y = (int)transform->position.y;
        destiation_rect.w = transform->width * transform->scale;
        destiation_rect.h = transform->height * transform->scale;
    }

    void update([[maybe_unused]] const float delta_time)
    {
        if (is_animated)
        {
            source_rect.x = source_rect.w * static_cast<int>((SDL_GetTicks64() / animation_speed) % frame_count);
        }

        source_rect.y = animation_index * transform->height;

        if (!is_fixed)
        {
            destiation_rect.x = (int)transform->position.x;
            destiation_rect.y = (int)transform->position.y;
            destiation_rect.w = transform->width * transform->scale;
            destiation_rect.h = transform->height * transform->scale;
        }
    }

    void render()
    {
        if (is_visible)
        {
            if (modulate)
            {
                SDL_SetTextureColorMod(texture, mod[0], mod[1], mod[2]);
                SDL_SetTextureAlphaMod(texture, mod[3]);
            }
            TextureManager::draw(texture, source_rect, destiation_rect, flip);
        }
    }

    void set_modulation(bool ext_mod, u_char r, u_char g, u_char b, u_char a)
    {
        modulate = ext_mod;
        mod[0] = r;
        mod[1] = g;
        mod[2] = b;
        mod[3] = a;
    }

    void debug_render()
    {
#ifdef DEBUG
        ImGui::Text("SpriteComponent: ");
        if (ImGui::TreeNode("Texture Info"))
        {
            ImGui::Text("texture_id: %s", Game::asset_manager->get_texture_name(texture).c_str());
            ImGui::TreePop();
        }
        ImGui::Text("Destination Rect: %d, %d", destiation_rect.x, destiation_rect.y);
        ImGui::Text("Animation Index: %d", animation_index);
        ImGui::Text("Frame Count: %d", frame_count);
        ImGui::DragInt("Animation Speed", &animation_speed, 1, 0, 1000);
        ImGui::Text("Current Animation: %s", current_animation_name.c_str());
        ImGui::Text("Is Animated: %s", is_animated ? "true" : "false");
#endif // DEBUG
    }

    std::string to_string() const
    {
        return std::string("Component<SpriteComponent>: ");
    }

private:
    TransformComponent *transform;
    SDL_Texture *texture;
    SDL_Rect source_rect;
    SDL_Rect destiation_rect;

    bool is_animated;
    bool is_fixed;

    int frame_count = 0;
    int animation_speed = 0;

    std::map<std::string, Animation> animations;

    std::string current_animation_name;

    unsigned int animation_index = 0;
};