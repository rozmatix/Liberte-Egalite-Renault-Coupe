#pragma once
#include "Vector2.h"
#include "Enemy.h"
#include <optional>
#include <random>

class Scene;

class EnemySpawner
{
public:
    EnemySpawner() = default;
    EnemySpawner(const Vector2& pos, unsigned int seed = std::random_device{}())
        : position(pos), rng(seed) {}

    void SetActive(bool a) { active = a; }
    bool IsActive() const { return active; }

    // dt: timestep. remainingTimeSec: time left until 0 (game ends). totalTimeSec: total game length (1800).
    void Update(float dt, float remainingTimeSec, float totalTimeSec, Scene& scene);

    Vector2 position{0.0f, 0.0f};

private:
    bool active = false;
    float timer = 0.0f;

    std::mt19937 rng;

    // Compute current spawn interval based on remaining time: 10s → 0.5s (linear with time to 0).
    float ComputeInterval(float remainingTimeSec, float totalTimeSec) const;

    // Get normalized weights per enemy class at current remaining time, via piecewise-linear interpolation.
    // Knot points given in minutes: 30, 22, 14, 6.
    struct Weights
    {
        float farmerNone;
        float farmerFork;
        float farmerTorch;
        float soldierRifle;
    };
    Weights ComputeWeights(float remainingTimeSec) const;

    // Randomly choose type+weapon using weights; weights assumed normalized to sum≈1.
    std::pair<EnemyType, std::optional<WeaponType>> ChooseEnemy(const Weights& w);

    void SpawnChosen(Scene& scene, EnemyType type, std::optional<WeaponType> weapon);
};