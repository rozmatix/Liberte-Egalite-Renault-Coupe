#pragma once
#include <vector>
#include "Sprite.h"
#include "Vector2.h"
#include <SDL3/SDL.h>

class SpriteBatch
{
public:
    SDL_Renderer* renderer;

    struct Entry { Sprite* sprite; Vector2 position; float angle; };
    std::vector<Entry> sprites;

    struct Circle { Vector2 position; float radius; SDL_Color color; };
    std::vector<Circle> circles;

    // Camera (focus-based)
    float cameraScale = 1.0f;
    Vector2 cameraFocus{ 0.0f, 0.0f };
    Vector2 cameraScreenCenter{ 0.0f, 0.0f };

    SpriteBatch(SDL_Renderer* renderer);

    void Add(Sprite* sprite, const Vector2& position, float angle);
    void AddCircle(const Vector2& position, float radius, SDL_Color color);

    // Debug oriented rectangle outline (red)
    void AddOrientedRectDebug(const Vector2& center, const Vector2& sizeWH, float angleDeg, SDL_Color color);

    void SetCameraScale(float scale);
    void SetCameraFocus(const Vector2& worldFocus, const Vector2& screenCenter);

    Vector2 WorldToScreen(const Vector2& worldPoint) const;
    Vector2 ScreenToWorld(float sx, float sy) const;

    void Draw();
};
