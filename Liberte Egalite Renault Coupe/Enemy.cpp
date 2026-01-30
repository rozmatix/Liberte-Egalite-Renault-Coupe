#include "Enemy.h"
#include "Player.h" // add this include so Player is a complete type
#include <algorithm>
#include <cmath>

Enemy::Enemy()
{
    position = Vector2(100, 100);
}

void Enemy::Init(Sprite* spr)
{
    sprite = spr;
}

void Enemy::Update(float dt)
{
    if (invincible)
    {
        invincibleTimer -= dt;
        if (invincibleTimer <= 0.0f)
        {
            invincible = false;
            invincibleTimer = 0.0f;
        }
    }
}

void Enemy::AI(Scene& scene, float dt)
{
    if (scene.player)
    {
        Vector2 toPlayer(
            scene.player->position.x - position.x,
            scene.player->position.y - position.y
        );

        float lenSq = toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y;
        if (lenSq > 0.0001f)
        {
            float len = std::sqrt(lenSq);
            position.x += (toPlayer.x / len) * speed * dt;
            position.y += (toPlayer.y / len) * speed * dt;
        }
    }
}

void Enemy::Draw(SpriteBatch* batch)
{
    if (sprite)
        batch->Add(sprite, position, /*angle*/ 0.0f);
}

void Enemy::Damage(int dmg)
{
    if (invincible) return;
    current_hp -= dmg;
    current_hp = std::max(0, current_hp);
    invincible = true;
    invincibleTimer = 0.5f;
}

void Enemy::Reset()
{
    current_hp = 40;
    position = Vector2(100, 100);
    invincible = false;
    invincibleTimer = 0.0f;
}