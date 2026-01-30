#include "SpriteBatch.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <cassert>
#include <cmath>

SpriteBatch::SpriteBatch(SDL_Renderer* renderer)
	: renderer(renderer)
{
}

void SpriteBatch::Add(Sprite* sprite, const Vector2& position, float angle)
{
    assert(sprite != nullptr);
    sprites.push_back({ sprite, position, angle });
}

void SpriteBatch::Draw()
{
	if (sprites.empty()) return;

    for (auto& entry : sprites)
    {
        Sprite* sprite = entry.sprite;
        const Vector2& position = entry.position;
        const float angleDeg = entry.angle;

        assert(sprite != nullptr);
        assert(sprite->texture != nullptr);
        assert(sprite->source.w > 0 && sprite->source.h > 0);
        assert(!std::isnan(position.x) && !std::isnan(position.y));

        float texW = 0, texH = 0;
        if (!SDL_GetTextureSize(sprite->texture, &texW, &texH)) 
            std::cerr << "SDL_GetTextureSize failed: " << SDL_GetError() << std::endl;

        SDL_FRect dst;
        dst.w = sprite->source.w;
        dst.h = sprite->source.h;
        dst.x = position.x - dst.w / 2.0f;
        dst.y = position.y - dst.h / 2.0f;

        SDL_FPoint center{ dst.w / 2.0f, dst.h / 2.0f };
        const SDL_FlipMode flip = SDL_FLIP_NONE;

        if (!SDL_RenderTextureRotated(renderer, sprite->texture, &sprite->source, &dst, angleDeg, &center, flip))
        {
            std::cerr << "SDL_RenderTextureRotated failed: " << SDL_GetError() << std::endl;
        }
    }
}