#pragma once
#include "Scene.h"
#include <functional>

class PhysicsEngine
{
public:
    void Simulate(Scene& scene, float dt);

    std::function<void(const Vector2& worldPos, int dmg)> onEnemyDamaged;

    void SetWorldBounds(float x, float y, float w, float h);

private:
    // OBB vs OBB test (centers, sizes, angles in degrees)
    static bool OBBvsOBB(const Vector2& c1, const Vector2& size1, float ang1Deg,
                         const Vector2& c2, const Vector2& size2, float ang2Deg);

    float worldX = 0.0f;
    float worldY = 0.0f;
    float worldW = 0.0f;
    float worldH = 0.0f;
};

