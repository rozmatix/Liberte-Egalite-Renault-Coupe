#include "Sprite.h"

Sprite::Sprite()
    : name(""), texture(nullptr), source{ 0,0,0,0 }, position(0, 0), angle(0.0f)
{
}

Sprite::Sprite(const std::string& name, SDL_Texture* tex, const SDL_FRect& src)
    : name(name), texture(tex), source(src), position(0, 0), angle(0.0f)
{
}