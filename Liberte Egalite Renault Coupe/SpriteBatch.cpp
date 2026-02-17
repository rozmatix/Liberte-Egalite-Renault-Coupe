#include "SpriteBatch.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <cassert>
#include <cmath>

static inline float DegToRad(float deg) { return deg * 3.14159265358979323846f / 180.0f; }

SpriteBatch::SpriteBatch(SDL_Renderer* renderer)
    : renderer(renderer)
{
}

void SpriteBatch::SetCameraScale(float scale)
{
    cameraScale = (scale > 0.0f) ? scale : 1.0f;
}

void SpriteBatch::SetCameraFocus(const Vector2& worldFocus, const Vector2& screenCenter)
{
    cameraFocus = worldFocus;
    cameraScreenCenter = screenCenter;
}

void SpriteBatch::Add(Sprite* sprite, const Vector2& position, float angle)
{
    assert(sprite != nullptr);
    sprites.push_back({ sprite, position, angle });
}

void SpriteBatch::AddCircle(const Vector2& position, float radius, SDL_Color color)
{
    circles.push_back({ position, radius, color });
}

Vector2 SpriteBatch::WorldToScreen(const Vector2& worldPoint) const
{
    Vector2 screen;
    screen.x = (worldPoint.x - cameraFocus.x) * cameraScale + cameraScreenCenter.x;
    screen.y = (worldPoint.y - cameraFocus.y) * cameraScale + cameraScreenCenter.y;
    return screen;
}

Vector2 SpriteBatch::ScreenToWorld(float sx, float sy) const
{
    Vector2 world;
    world.x = (sx - cameraScreenCenter.x) / cameraScale + cameraFocus.x;
    world.y = (sy - cameraScreenCenter.y) / cameraScale + cameraFocus.y;
    return world;
}

void SpriteBatch::AddOrientedRectDebug(const Vector2& center, const Vector2& sizeWH, float angleDeg, SDL_Color color)
{
    // compute local corners
    const float hw = sizeWH.x * 0.5f;
    const float hh = sizeWH.y * 0.5f;
    const Vector2 local[4] = {
        Vector2(-hw, -hh),
        Vector2(hw, -hh),
        Vector2(hw, hh),
        Vector2(-hw, hh)
    };
    const float rad = DegToRad(angleDeg);
    const float cs = std::cos(rad), sn = std::sin(rad);

    SDL_FPoint pts[5];
    for (int i = 0; i < 4; ++i)
    {
        // rotate local corner and translate to world
        const float rx = local[i].x * cs - local[i].y * sn;
        const float ry = local[i].x * sn + local[i].y * cs;
        const Vector2 worldPos(center.x + rx, center.y + ry);
        const Vector2 screenPos = WorldToScreen(worldPos);
        pts[i].x = screenPos.x;
        pts[i].y = screenPos.y;
    }
    pts[4] = pts[0]; // close loop

    // draw red outline
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderLines(renderer, pts, 5);
}

void SpriteBatch::Draw()
{
    if (sprites.empty() && circles.empty()) return;

    for (auto& entry : sprites)
    {
        Sprite* sprite = entry.sprite;
        const Vector2 worldPos = entry.position;
        const float angleDeg = entry.angle;

        // Camera mapping
        const float vx = (worldPos.x - cameraFocus.x) * cameraScale + cameraScreenCenter.x;
        const float vy = (worldPos.y - cameraFocus.y) * cameraScale + cameraScreenCenter.y;

        SDL_FRect dst;
        dst.w = sprite->source.w * cameraScale;
        dst.h = sprite->source.h * cameraScale;
        dst.x = vx - dst.w * 0.5f;
        dst.y = vy - dst.h * 0.5f;

        SDL_FPoint center{ dst.w * 0.5f, dst.h * 0.5f };



        if (!SDL_RenderTextureRotated(renderer, sprite->texture, &sprite->source, &dst, angleDeg, &center, SDL_FLIP_NONE))
        {
            std::cerr << "SDL_RenderTextureRotated failed: " << SDL_GetError() << std::endl;
        }
    }

    // Circles (geometry fan)
    for (const auto& c : circles)
    {
        const float cx = (c.position.x - cameraFocus.x) * cameraScale + cameraScreenCenter.x;
        const float cy = (c.position.y - cameraFocus.y) * cameraScale + cameraScreenCenter.y;
        const float rr = c.radius * cameraScale;

        const int segments = 24;
        std::vector<SDL_Vertex> verts;
        std::vector<int> indices;
        verts.reserve(segments + 2);
        indices.reserve(segments * 3);

        const SDL_FColor color = {
            c.color.r / 255.0f,
            c.color.g / 255.0f,
            c.color.b / 255.0f,
            c.color.a / 255.0f
        };

        SDL_Vertex center{};
        center.position.x = cx;
        center.position.y = cy;
        center.color = color;
        center.tex_coord.x = 0.0f; center.tex_coord.y = 0.0f;
        verts.push_back(center);

        for (int i = 0; i <= segments; ++i)
        {
            float t = (float)i / segments * 2.0f * 3.14159265359f;
            float x = cx + std::cos(t) * rr;
            float y = cy + std::sin(t) * rr;

            SDL_Vertex v{};
            v.position.x = x;
            v.position.y = y;
            v.color = color;
            v.tex_coord.x = 0.0f; v.tex_coord.y = 0.0f;
            verts.push_back(v);
        }

        for (int i = 1; i <= segments; ++i)
        {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i + 1);
        }

        if (!SDL_RenderGeometry(renderer, nullptr, verts.data(), (int)verts.size(), indices.data(), (int)indices.size()))
        {
            std::cerr << "SDL_RenderGeometry failed: " << SDL_GetError() << std::endl;
        }
    }

    sprites.clear();
    circles.clear();
}