#include "EnemySpawner.h"
#include "Scene.h"
#include <algorithm>
#include <cmath>

static inline float Clamp01(float v) { return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v); }
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }

float EnemySpawner::ComputeInterval(float remainingTimeSec, float totalTimeSec) const
{
    const float startInterval = 5.0f;
    const float endInterval = 0.2f;
    const float t = Clamp01((totalTimeSec - remainingTimeSec) / totalTimeSec); // 0 at start, 1 at end
    return Lerp(startInterval, endInterval, t);
}

// Piecewise linear interpolation for weights across remaining time knots: 30→22→14→6 minutes.
// Values per spec:
// farmerNone:   1.00 → 0.50 → 0.15 → 0.00
// farmerFork:   0.00 → 0.25 → 0.40 → 0.25
// farmerTorch:  0.00 → 0.15 → 0.25 → 0.25
// soldierRifle: 0.00 → 0.10 → 0.20 → 0.50
EnemySpawner::Weights EnemySpawner::ComputeWeights(float remainingTimeSec) const
{
    const float k0 = 30.0f * 60.0f;
    const float k1 = 22.0f * 60.0f;
    const float k2 = 14.0f * 60.0f;
    const float k3 = 6.0f  * 60.0f;

    auto segmentInterp = [](float tSec,
                            float tA, float vA,
                            float tB, float vB,
                            float tC, float vC,
                            float tD, float vD)
    {
        if (tSec >= tA) return vA;
        if (tSec >= tB) {
            const float t = Clamp01((tA - tSec) / (tA - tB));
            return Lerp(vA, vB, t);
        }
        if (tSec >= tC) {
            const float t = Clamp01((tB - tSec) / (tB - tC));
            return Lerp(vB, vC, t);
        }
        if (tSec >= tD) {
            const float t = Clamp01((tC - tSec) / (tC - tD));
            return Lerp(vC, vD, t);
        }
        // below last knot => hold last value
        return vD;
    };

    Weights w{};
    w.farmerNone  = segmentInterp(remainingTimeSec, k0, 1.0f, k1, 0.5f, k2, 0.15f, k3, 0.0f);
    w.farmerFork  = segmentInterp(remainingTimeSec, k0, 0.0f, k1, 0.25f, k2, 0.40f, k3, 0.25f);
    w.farmerTorch = segmentInterp(remainingTimeSec, k0, 0.0f, k1, 0.15f, k2, 0.25f, k3, 0.25f);
    w.soldierRifle= segmentInterp(remainingTimeSec, k0, 0.0f, k1, 0.10f, k2, 0.20f, k3, 0.50f);

    // Normalize to sum to 1
    const float sum = std::max(0.0001f, w.farmerNone + w.farmerFork + w.farmerTorch + w.soldierRifle);
    w.farmerNone   /= sum;
    w.farmerFork   /= sum;
    w.farmerTorch  /= sum;
    w.soldierRifle /= sum;
    return w;
}

std::pair<EnemyType, std::optional<WeaponType>> EnemySpawner::ChooseEnemy(const Weights& w)
{
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    const float r = dist(rng);

    float acc = w.farmerNone;
    if (r < acc) return { EnemyType::Farmer, std::optional<WeaponType>(WeaponType::None) };

    acc += w.farmerFork;
    if (r < acc) return { EnemyType::Farmer, std::optional<WeaponType>(WeaponType::Pitchfork) };

    acc += w.farmerTorch;
    if (r < acc) return { EnemyType::Farmer, std::optional<WeaponType>(WeaponType::Torch) };

    // Else soldier with rifle
    return { EnemyType::Soldier, std::optional<WeaponType>(WeaponType::Rifle) };
}

void EnemySpawner::SpawnChosen(Scene& scene, EnemyType type, std::optional<WeaponType> weapon)
{
    if (type == EnemyType::Farmer)
        scene.enemies.emplace_back(Enemy(EnemyType::Farmer, position));
    else
        scene.enemies.emplace_back(Enemy(EnemyType::Soldier, position));

    Enemy& e = scene.enemies.back();
    // Base sprite indices: farmer=0, soldier=2 (matches your Gameplay spawns)
    // Weapon sprites are optional; logic works without them.
    // Assign base sprite via Gameplay during enemy initialization elsewhere; here we just set weapon.
    e.SetWeapon(weapon);
}

void EnemySpawner::Update(float dt, float remainingTimeSec, float totalTimeSec, Scene& scene)
{
    if (!active) return;

    timer += dt;
    const float interval = ComputeInterval(remainingTimeSec, totalTimeSec);
    if (timer >= interval)
    {
        timer = 0.0f;
        const Weights w = ComputeWeights(remainingTimeSec);
        auto choice = ChooseEnemy(w);
        SpawnChosen(scene, choice.first, choice.second);
    }
}