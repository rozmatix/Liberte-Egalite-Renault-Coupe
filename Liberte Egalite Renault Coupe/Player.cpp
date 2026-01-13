#include "Player.h"
#include <iostream>

Player::Player()
{
    position = Vector2(300, 300);
}

void Player::Init(Sprite* spr)
{
    sprite = spr;
}

void Player::HandleInput(bool keyState[], float dt)
{
    if (keyState[SDL_SCANCODE_W]) position.y -= speed * dt;
    if (keyState[SDL_SCANCODE_S]) position.y += speed * dt;
    if (keyState[SDL_SCANCODE_A]) position.x -= speed * dt;
    if (keyState[SDL_SCANCODE_D]) position.x += speed * dt;

    if (sprite)
        sprite->position = position;
}

void Player::Update(float dt)
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

void Player::Draw(SpriteBatch* batch)
{
    if (sprite)
        batch->Add(sprite);
}

void Player::Damage(int dmg)
{
    if (invincible) return;

    current_hp -= dmg;
    if (current_hp < 0) current_hp = 0;

    invincible = true;
    invincibleTimer = 1.0f;
}

void Player::Reset()
{
    current_hp = 100;
    max_hp = 100;
    position = Vector2(300, 300);
    invincible = false;
    invincibleTimer = 0.0f;
}