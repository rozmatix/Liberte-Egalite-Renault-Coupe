#include "Enemy.h"
#include "SpriteBatch.h"
#include "Sprite.h"
#include "Scene.h"
#include "Player.h"
#include "Bullet.h"
#include "Torch.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <random>

static inline float DegToRad(float d) { return d * 3.14159265358979323846f / 180.0f; }

Enemy::Enemy()
{
    position = Vector2(0.0f, 0.0f);
    current_hp = 40;
    angle = 0.0f;
    weapon = std::nullopt;
    type = EnemyType::Farmer;
    baseSprite = nullptr;
    weaponSprite = nullptr;
    fireCooldown = 0.0f;
}

Enemy::Enemy(EnemyType t, const Vector2& spawnPos)
{
    type = t;
    position = spawnPos;
    current_hp = 40;
    angle = 0.0f;
    weapon = std::nullopt;
    baseSprite = nullptr;
    weaponSprite = nullptr;
    fireCooldown = 0.0f;
}

void Enemy::Init(Sprite* s) { baseSprite = s; }
void Enemy::Update(float dt) { (void)dt; }

void Enemy::AI(Scene& scene, float dt)
{
    if (scene.player)
    {
        const Vector2 toPlayer{ scene.player->position.x - position.x, scene.player->position.y - position.y };
        const float lenSq = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
        if (lenSq > 0.0001f)
        {
            const float len = std::sqrt(lenSq);
            Vector2 dir{ toPlayer.x / len, toPlayer.y / len };

            float speed = (type == EnemyType::Soldier) ? 140.0f : 110.0f;
            // Apply frost slow
            if (IsFrosted())
            {
                DecreaseFrostTime(dt);
                speed *= (1.0f - GetFrostSlowPercent());
            }

            position.x += dir.x * speed * dt;
            position.y += dir.y * speed * dt;

            angle = std::atan2(dir.y, dir.x) * 180.0f / 3.14159265358979323846f + 90.0f;
        }
    }

    // Farmer torch throw behavior
    if (type == EnemyType::Farmer && weapon.has_value() && weapon.value() == WeaponType::Torch && scene.player)
    {
        const Vector2 toPlayer{ scene.player->position.x - position.x, scene.player->position.y - position.y };
        const float distSq = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;

        // Reasonable throw range so it triggers without needing to be on top of the player
        const float throwDist = 1600.0f; // extended detection range so farmers throw from farther away
        if (distSq <= throwDist * throwDist)
        {
            // Compute current hand/world weapon mount position so the torch starts from the hand
            const Vector2 local = GetHandLocalOffset();
            const float rad = DegToRad(angle);
            const float cs = std::cos(rad), sn = std::sin(rad);
            const Vector2 offset(local.x * cs - local.y * sn, local.x * sn + local.y * cs);
            const Vector2 startPos(position.x + offset.x, position.y + offset.y);

            // Random point around player using independent x/y errors instead of a radial scatter
            std::random_device rd;
            std::mt19937 rng(rd());
            // Tune the error range as needed; ±40 units gives a natural miss without overshooting
            std::uniform_real_distribution<float> xErrDist(-40.0f, 40.0f);
            std::uniform_real_distribution<float> yErrDist(-40.0f, 40.0f);

            const Vector2 target(
                scene.player->position.x + xErrDist(rng),
                scene.player->position.y + yErrDist(rng)
            );

            Torch t;
            // Start from the enemy hand and aim at player with XY error
            t.InitThrow(startPos, target, weaponSprite);
            scene.torches.push_back(t);

            // Enemy loses the torch after throwing
            SetWeapon(std::nullopt);
        }
    }

    // Soldier rifle shooting
    if (type == EnemyType::Soldier && weapon.has_value() && weapon.value() == WeaponType::Rifle && scene.player)
    {
        fireCooldown = std::max(0.0f, fireCooldown - dt);

        const Vector2 toPlayer{ scene.player->position.x - position.x, scene.player->position.y - position.y };
        const float lenSq = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
        if (lenSq > 0.0001f && fireCooldown <= 0.0f)
        {
            const float len = std::sqrt(lenSq);
            const Vector2 dir{ toPlayer.x / len, toPlayer.y / len };

            Bullet b;
            b.Init(position, dir, true);
            scene.bullets.push_back(b);

            fireCooldown = 2.0f;
        }
    }
}

Vector2 Enemy::HitboxSize() const
{
    const float widthPct = 0.55f;
    const float heightPct = 0.15f;
    if (baseSprite)
        return Vector2(baseSprite->source.w * widthPct, baseSprite->source.h * heightPct);
    return Vector2(44.0f, 44.0f);
}

float Enemy::AngleDeg() const { return angle; }

int Enemy::Damage() const
{
    switch (GetWeapon())
    {
    case WeaponType::Rifle:     return 20;
    case WeaponType::Pitchfork: return 15;
    case WeaponType::Torch:     return 12;
    default:                    return 8;
    }
}

int Enemy::Damage(int amount)
{
    if (amount <= 0 || current_hp <= 0) return 0;
    const int oldHP = current_hp;
    current_hp = std::max(0, current_hp - amount);
    std::cout << "Enemy took " << amount << " damage, HP now " << current_hp << "\n";
    return current_hp < oldHP ? 1 : 0;
}

WeaponType Enemy::SanitizeWeaponForType(WeaponType w) const
{
    if (w == WeaponType::None) return WeaponType::None;
    if (type == EnemyType::Farmer)
        return (w == WeaponType::Torch || w == WeaponType::Pitchfork) ? w : WeaponType::None;
    else
        return (w == WeaponType::Rifle) ? w : WeaponType::None;
}

void Enemy::SetWeapon(std::optional<WeaponType> w)
{
    if (!w.has_value()) { weapon = std::nullopt; weaponSprite = nullptr; return; }
    const WeaponType sanitized = SanitizeWeaponForType(w.value());
    if (sanitized == WeaponType::None) { weapon = std::nullopt; weaponSprite = nullptr; }
    else { weapon = sanitized; }
}

int Enemy::GetWeaponSpriteIndex() const
{
    const WeaponType w = GetWeapon();
    switch (w)
    {
    case WeaponType::Torch:     return 3;
    case WeaponType::Rifle:     return 4;
    case WeaponType::Pitchfork: return 5;
    default:                    return -1;
    }
}

Vector2 Enemy::GetHandLocalOffset() const
{
    float offsetXPct = (type == EnemyType::Soldier) ? 0.20f : 0.25f;
    float offsetYPct = (type == EnemyType::Soldier) ? -0.12f : -0.15f;
    if (baseSprite)
        return Vector2(baseSprite->source.w * offsetXPct, baseSprite->source.h * offsetYPct);
    return Vector2(10.0f, -6.0f);
}

void Enemy::Draw(SpriteBatch* batch)
{
    if (!batch) return;

    const Vector2 local = GetHandLocalOffset();
    const float rad = DegToRad(angle);
    const float cs = std::cos(rad), sn = std::sin(rad);
    const Vector2 offset(local.x * cs - local.y * sn, local.x * sn + local.y * cs);
    const Vector2 weaponPos(position.x + offset.x, position.y + offset.y);

    if (weapon.has_value() && weapon.value() != WeaponType::None && weaponSprite)
        batch->Add(weaponSprite, weaponPos, angle);

    if (baseSprite)
        batch->Add(baseSprite, position, angle);

    const Vector2 hb = HitboxSize();
    batch->AddOrientedRectDebug(position, hb, angle, SDL_Color{ 255, 0, 0, 255 });
}