#pragma once
#include <vector>
#include <string>

struct EnemySave
{
    float x;
    float y;
    int   hp;

    // serialized enums
    int   type;      // EnemyType kot int
    int   weapon;    // WeaponType kot int
    float angle;     // smer sovražnika
};

struct BulletSave { float x, y, dx, dy; };
struct TorchSave { float x, y; bool lit; };
struct XpOrbSave { float x, y; int xpAmount; };

struct SaveData
{
    bool SoundOn = true;

    float playerX = 0.0f;
    float playerY = 0.0f;
    int   playerHP = 0;

    // New: persist player heading
    float playerAngle = -90.0f;

    float gameTime = 0.0f;

    std::vector<EnemySave> enemies;
    std::vector<BulletSave> bullets;
    std::vector<TorchSave> torches;
    std::vector<XpOrbSave> xpOrbs;
};

namespace SaveManager
{
    bool Save(const SaveData& data);
    bool Load(SaveData& out);
}