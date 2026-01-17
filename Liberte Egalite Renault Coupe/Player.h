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

    void Init(Sprite* sprite);
    void HandleInput(bool keyState[], float dt); // dt dodan
    void Update(float dt);
    void Draw(SpriteBatch* batch);
    void Damage(int dmg);
    void Reset();

    Vector2 position;
    float speed = 200.0f;
    int current_hp = 100;

    Sprite* sprite = nullptr;

private:

    int max_hp = 100;
    bool invincible = false;
    float invincibleTimer = 0.0f;
};