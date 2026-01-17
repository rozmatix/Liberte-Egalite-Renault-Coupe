#pragma once
#include <string>
#include <vector>

struct EnemySave
{
    float x = 0.0f;
    float y = 0.0f;
    int hp = 0;
};

struct SaveData
{
    bool SoundOn = true;
    float playerX = 0.0f;
    float playerY = 0.0f;
    int playerHP = 0;
    float gameTime = 0.0f;

    std::vector<EnemySave> enemies;
};

class SaveManager
{
public:
    // Load returns true if file found and parsed
    static bool Load(SaveData& out);

    // Save returns true on success
    static bool Save(const SaveData& data);

    // Get the actual path used for debugging
    static std::string GetSaveFilePath(const std::string& filename = "save.txt");
};