#pragma once
#include "Vector2.h"
#include <optional>

class SpriteBatch;
class Sprite;
class Scene;

enum class EnemyType { Farmer, Soldier };
enum class WeaponType { None, Torch, Rifle, Pitchfork };

class Enemy
{
public:
    Enemy();
    Enemy(EnemyType type, const Vector2& spawnPos);

    Vector2 position;
    int current_hp = 40;
    float angle = 0.0f;

    void Init(Sprite* baseSprite);
    void Update(float dt);
    void Draw(SpriteBatch* batch);
    void AI(Scene& scene, float dt);

    Vector2 HitboxSize() const;
    float AngleDeg() const;

    int Damage() const;        // base damage by weapon (query)
    int Damage(int amount);    // apply damage (mutates HP), returns 1 if applied

    EnemyType GetType() const { return type; }
    WeaponType GetWeapon() const { return weapon.value_or(WeaponType::None); }
    void SetWeapon(std::optional<WeaponType> w);
    void SetWeaponSprite(Sprite* s) { weaponSprite = s; }

    // Burning state API (public, minimal)
    void Ignite(float durationSeconds, int tickDamage)
    {
        if (durationSeconds <= 0.0f || tickDamage <= 0) return;
        burnTimeRemaining = durationSeconds;   // refresh timer
        burnTickAccum = 0.0f;                  // reset accumulator
        burnTickDamage = tickDamage;           // set per-tick damage
    }

    bool IsBurning() const { return burnTimeRemaining > 0.0f; }
    float GetBurnTimeRemaining() const { return burnTimeRemaining; }
    void AddBurnAccum(float dt) { burnTickAccum += dt; }
    void ResetBurnAccum() { burnTickAccum = 0.0f; }
    float GetBurnAccum() const { return burnTickAccum; }
    int GetBurnTickDamage() const { return burnTickDamage; }
    void DecreaseBurnTime(float dt)
    {
        burnTimeRemaining = (burnTimeRemaining > dt) ? (burnTimeRemaining - dt) : 0.0f;
    }

    // Frost slow state API
    void ApplyFrost(float durationSeconds, float slowPercent)
    {
        if (durationSeconds <= 0.0f || slowPercent <= 0.0f) return;
        frostTimeRemaining = durationSeconds;      // refresh timer
        frostSlowPercent   = slowPercent;          // 0.1 for 10% slow, stacks additive
    }
    bool IsFrosted() const { return frostTimeRemaining > 0.0f; }
    void DecreaseFrostTime(float dt)
    {
        frostTimeRemaining = (frostTimeRemaining > dt) ? (frostTimeRemaining - dt) : 0.0f;
        if (frostTimeRemaining <= 0.0f) frostSlowPercent = 0.0f;
    }
    float GetFrostSlowPercent() const { return frostSlowPercent; }

private:
    EnemyType type = EnemyType::Farmer;
    std::optional<WeaponType> weapon;
    Sprite* baseSprite = nullptr;
    Sprite* weaponSprite = nullptr;

    float fireCooldown = 0.0f;

    // Burning state (kept private; accessed via the API above)
    float burnTimeRemaining = 0.0f;
    float burnTickAccum = 0.0f;
    int   burnTickDamage = 0;

    float frostTimeRemaining = 0.0f;
    float frostSlowPercent   = 0.0f; // cumulative; e.g., 0.1f, 0.2f, ...
    int GetBaseSpriteIndex() const;
    int GetWeaponSpriteIndex() const;
    Vector2 GetHandLocalOffset() const;
    WeaponType SanitizeWeaponForType(WeaponType w) const;
};