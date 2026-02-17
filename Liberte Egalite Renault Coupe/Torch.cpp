#include "Torch.h"
#include "SpriteBatch.h"
#include <cmath>

static inline float Clamp(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

void Torch::InitThrow(const Vector2& from, const Vector2& target, Sprite* torchSprite)
{
    sprite = torchSprite;
    position = from;
    const Vector2 to(target.x - from.x, target.y - from.y);
    const float len = std::sqrt(to.x * to.x + to.y * to.y);
    Vector2 dir{0.0f, 0.0f};
    if (len > 0.0001f)
        dir = Vector2(to.x / len, to.y / len);

    // Reduce speed so it doesn't overshoot far past the player
    const float throwSpeed = 320.0f; // was 800.0f
    velocity = Vector2(dir.x * throwSpeed, dir.y * throwSpeed);
    angleDeg = 0.0f;
    angularSpeedDeg = 720.0f;
    airborne = true;
    dead = false;
    hazardTickAccum = 0.0f;
    airborneTime = 0.0f;
    groundLifeAccum = 0.0f;
}

void Torch::Update(float dt)
{
    if (dead) return;

    if (airborne)
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        angleDeg += angularSpeedDeg * dt;

        // Stronger drag to slow down sooner
        const float dragPerSecond = 0.9f; // 10% per second
        velocity.x *= std::pow(dragPerSecond, dt);
        velocity.y *= std::pow(dragPerSecond, dt);

        airborneTime += dt;
        const float speedSq = velocity.x * velocity.x + velocity.y * velocity.y;

        // Shorter flight duration so it lands near target
        if (airborneTime >= 1.4f || speedSq < 9.0f)
        {
            airborne = false;
            velocity = Vector2(0.0f, 0.0f);
            angularSpeedDeg = 0.0f;
        }
    }
    else
    {
        groundLifeAccum += dt;
        if (groundLifeAccum >= groundLifeTime)
        {
            dead = true;
            return;
        }

        hazardTickAccum = Clamp(hazardTickAccum, 0.0f, hazardTickInterval);
    }
}

void Torch::Draw(SpriteBatch* batch) const
{
    if (!batch) return;

    if (sprite)
    {
        // Render sprite rotating around its center
        batch->Add(sprite, position, angleDeg);
    }
    else
    {
        // Fallback debug circle if sprite missing
        SDL_Color col = airborne ? SDL_Color{255, 140, 0, 255} : SDL_Color{255, 64, 0, 255};
        const float r = airborne ? throwRadius : hazardRadius;
        batch->AddCircle(position, r, col);
    }
}