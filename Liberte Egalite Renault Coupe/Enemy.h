#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "Vector2.h"
#include "Sprite.h"
#include "SpriteBatch.h"
#include "Scene.h"

class Enemy
{
public:
    Enemy();


    void Init(Sprite* sprite);
    void Update(float dt);
    void Draw(SpriteBatch* batch);
    void AI(Scene& scene, float dt);
    void Damage(int dmg);
    void Reset();

    Vector2 position;
    float speed = 200.0f;
    int current_hp = 40;

private:
    
    Sprite* sprite = nullptr;
    bool invincible = false;
    float invincibleTimer = 0.0f;
};

