#pragma once
#include <vector>
#include "Vector2.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Torch.h"

struct XpOrb
{
    Vector2 position;
    float radius = 14.0f;
    int xpAmount = 10;
};

class SpriteBatch;
class Scene
{
public:
    Player* player = nullptr;
    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Torch> torches;
    std::vector<XpOrb> xpOrbs;

    void Update(float dt);
    void Draw(SpriteBatch* batch);
};