#include "SpriteBatch.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <cassert>

SpriteBatch::SpriteBatch(SDL_Renderer* renderer)
	: renderer(renderer)
{
}


void SpriteBatch::Add(Sprite* sprite)
{
	sprites.push_back(sprite);
}


void SpriteBatch::Draw()
{
	if (sprites.empty()) return;


    for (auto sprite : sprites)
    {
        assert(sprite != nullptr);
        assert(sprite->texture != nullptr);
        assert(sprite->source.w > 0 && sprite->source.h > 0);
        assert(!std::isnan(sprite->position.x) && !std::isnan(sprite->position.y));


        float texW = 0, texH = 0;
        if (!SDL_GetTextureSize(sprite->texture, &texW, &texH)) 
            std::cerr << "SDL_GetTextureSize failed: " << SDL_GetError() << std::endl;



        SDL_FRect dst;
        dst.w = sprite->source.w;
        dst.h = sprite->source.h;
        dst.x = sprite->position.x - dst.w / 2.0f;
        dst.y = sprite->position.y - dst.h / 2.0f;

        if (!SDL_RenderTexture(renderer, sprite->texture, &sprite->source, &dst) != 0)
        {
            std::cerr << "SDL_RenderTexture failed: " << SDL_GetError() << std::endl;
        }
    }


}