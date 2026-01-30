#include "Player.h"
#include <iostream>
#include <cmath>

static inline float DegToRad(float deg) { return deg * 3.14159265358979323846f / 180.0f; }

Player::Player()
{
    position = Vector2(300, 300);
    angle = -90.0f; // default heading up
}

void Player::Init(Sprite* spr)
{
    sprite = spr;
}

void Player::HandleInput(bool keyState[], float dt)
{
    // Rotate left/right (A/D)
    if (keyState[SDL_SCANCODE_A]) angle -= turnSpeed * dt;
    if (keyState[SDL_SCANCODE_D]) angle += turnSpeed * dt;

    // Move forward/back (W/S) along current heading
    float rad = DegToRad(angle);
    Vector2 forward(std::cos(rad), std::sin(rad));

    // Swap the direction to match expected screen-space movement
    if (keyState[SDL_SCANCODE_W]) {
        position.x -= forward.x * speed * dt; // forward
        position.y -= forward.y * speed * dt;
    }
    if (keyState[SDL_SCANCODE_S]) {
        position.x += forward.x * speed * dt; // backward
        position.y += forward.y * speed * dt;
    }
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
    {
        // Visual offset so the texture points forward (up) relative to movement
        const float visualAngleOffset = -90.0f;
        batch->Add(sprite, position, angle + visualAngleOffset);
    }
}

bool Player::Damage(int dmg)
{
    if (invincible) return false;

    current_hp -= dmg;
    if (current_hp < 0) current_hp = 0;

    invincible = true;
    invincibleTimer = 1.0f;
    return true;
}

void Player::Reset()
{
    current_hp = 100;
    max_hp = 100;
    position = Vector2(300, 300);
    angle = -90.0f; // reset heading up
    invincible = false;
    invincibleTimer = 0.0f;
}