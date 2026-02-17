#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "Vector2.h"
#include "Sprite.h"
#include "SpriteBatch.h"

class Player
{
public:
    Player();

    void Init(Sprite* spr);
    void HandleInput(bool keyState[], float dt);
    void Update(float dt);
    void Draw(SpriteBatch* batch);

    bool Damage(int dmg);
    void ApplyHazardDamage(int dmg);
    void Reset();

    // Transform
    Vector2 position;
    float angle = 0.0f;

    // Movement tuning
    float turnSpeed = 180.0f;
    float maxSpeed = 450.0f;
    float accel = 800.0f;
    float brakeAccel = 1200.0f;
    float reverseMax = 220.0f;
    float reverseAccel = 500.0f;
    float coastDrag = 300.0f;
    float speedScalar = 0.0f;

    // Health
    int current_hp = 20;
    int max_hp = 20;

    // Visual
    Sprite* sprite = nullptr;

    // Gun support
    void SetGunSprite(Sprite* gun) { gunSprite = gun; }
    void SetGunOffsetPercent(float xPct, float yPct) { gunOffsetXPct = xPct; gunOffsetYPct = yPct; }
    Vector2 GetGunMuzzleWorld(const Vector2& mouseWorld) const;

    // Firing control
    bool CanFire() const { return fireCooldown <= 0.0f; }
    void ConsumeFire() { fireCooldown = fireRate; }

    // Invincibility controls
    bool IsInvincible() const { return invincible; }
    void StartInvincibility(float seconds) { invincible = true; invincibleTimer = seconds; }

    // XP/Level mechanics
    void GainXp(int amount);
    int GetLevel() const { return level; }
    int GetXp() const { return xp; }
    int GetXpRequired() const { return xpRequired; }

    // Hitbox helpers
    Vector2 HitboxSize() const;
    float AngleDeg() const;
    float ColliderRadius() const;

    float GetFireRate() const { return fireRate; }
    void SetFireRate(float r) { fireRate = (r > 0.05f) ? r : 0.05f; } // clamp

    void SetMaxSpeedMultiplier(float mult)
    {
        maxSpeed *= 1.1f;       // +10% per pickup
        reverseMax *= 1.1f;     // keep reverse coherent
    }

    void IncreaseMaxHp(int amount)
    {
        max_hp += amount;
        current_hp = std::min(max_hp, current_hp + amount);
    }

    void Heal(int amount)
    {
        current_hp = std::min(max_hp, current_hp + amount);
    }

private:
    bool invincible = false;
    float invincibleTimer = 0.0f;

    float gunOffsetXPct = 0.10f;
    float gunOffsetYPct = -0.10f;
    Sprite* gunSprite = nullptr;

    float fireRate = 0.8f;
    float fireCooldown = 0.0f;

    // XP/Level fields
    int level = 1;
    int xp = 0;
    int xpRequired = 50;

    void RecomputeXpRequired();
    void LevelUp();
};