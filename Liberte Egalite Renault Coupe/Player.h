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

    // Return true if damage was applied, false if ignored (e.g., invincible)
    bool Damage(int dmg);
    void Reset();

    Vector2 position;
    float speed = 200.0f;     // forward/backward speed in pixels/sec
    float turnSpeed = 180.0f; // degrees/sec for rotation
    float angle = 0.0f;       // heading angle in degrees, 0 pointing right, positive clockwise
    int current_hp = 100;
    int max_hp = 100;

    Sprite* sprite = nullptr;

private:
    bool invincible = false;
    float invincibleTimer = 0.0f;
};