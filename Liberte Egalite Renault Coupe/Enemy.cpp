#include "Enemy.h"
#include <iostream>
#include "Player.h"

Enemy::Enemy()
{
}

void Enemy::Init(Sprite* spr)
{
    sprite = spr;
}

void Enemy::AI(Scene& scene, float dt)
{
    if (!scene.player) return;

    Vector2 direction = scene.player->position - position;

    float length = direction.Length();
    if (length > 1.0f)
    {

        position = position + (direction.Normalized() * speed * dt);
    }

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
    if (sprite)
        sprite->position = position;

}

void Enemy::Draw(SpriteBatch* batch)
{
    if (sprite)
        batch->Add(sprite);
}

void Enemy::Damage(int dmg)
{
    if (invincible) return; // èe je invincible, ne naredi niè

    current_hp -= dmg;
    std::cout << current_hp << " sovraznik" << std::endl;
    if (current_hp < 0) current_hp = 0;

    invincible = true;
    invincibleTimer = 1.0f; // 1 sekunda invincibility
}

void Enemy::Reset()
{
    current_hp = 40;
    position = Vector2(600, 300);
    invincible = false;
    invincibleTimer = 0.0f;
}