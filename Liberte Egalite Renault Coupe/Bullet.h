#pragma once
#include "Vector2.h"
#include "Sprite.h"
#include <unordered_set>

class SpriteBatch;
class Enemy;

class Bullet
{
public:
    Vector2 position{0.0f, 0.0f};
    Vector2 velocity{0.0f, 0.0f};
    float life = 2.0f;
    float radius = 6.0f;
    bool fromEnemy = false;

    // Visuals
    Sprite* sprite = nullptr;
    float angleDeg = 0.0f;

    // Pierce (player bullets only): number of extra enemies the bullet can hit
    // 0 = current behavior (die on first enemy hit), 1 = pass through one extra enemy, etc.
    int pierceRemaining = 0;

    // Track which enemies this bullet already damaged to avoid multiple damage applications
    std::unordered_set<const Enemy*> hitEnemies;

    // Fire power: level determines DoT per tick (2 * level). 0 = no fire.
    int fireLevel = 0;
    int powerBonus = 0;
    int frostLevel = 0; // add frost slow level

    void Init(const Vector2& start, const Vector2& dir, bool fromEnemyBullet, int pierceExtra = 0);
    void Update(float dt);
    void Draw(SpriteBatch* batch) const;

    bool IsDead() const { return life <= 0.0f; }

    // Hit tracking helpers
    bool CanHitEnemy(const Enemy* e) const;
    void RegisterEnemyHit(const Enemy* e);
};

