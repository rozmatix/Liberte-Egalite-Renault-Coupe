#pragma once
#include "Vector2.h"
#include "Sprite.h"

class SpriteBatch;
class Player;

class Torch
{
public:
    // Visual
    Sprite* sprite = nullptr;

    // State
    Vector2 position{0.0f, 0.0f};
    Vector2 velocity{0.0f, 0.0f};
    float angleDeg = 0.0f;
    float angularSpeedDeg = 720.0f; // spins fast while airborne
    bool airborne = true;
    bool dead = false;

    // Collision sizes
    float throwRadius = 12.0f;     // airborne collision vs player
    float hazardRadius = 18.0f;    // ground hazard radius

    // Damage
    int impactDamage = 12;         // damage when hitting player mid-air
    int hazardTickDamage = 5;      // damage per hazard tick
    float hazardTickInterval = 0.5f;

    // Timers
    float hazardTickAccum = 0.0f;
    float airborneTime = 0.0f;
    float maxAirborneTime = 1.8f;  // lands after this if not hitting player
    float groundLifeAccum = 0.0f;
    float groundLifeTime = 10.0f;  // despawn after 10s on ground

    // Initialization and update
    void InitThrow(const Vector2& from, const Vector2& target, Sprite* torchSprite);
    void Update(float dt);

    // Helpers
    bool IsDead() const { return dead; }
    bool IsAirborne() const { return airborne; }

    // Draw
    void Draw(SpriteBatch* batch) const;
};