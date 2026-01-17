#include "SaveManager.h"
#include <SDL3/SDL.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

static std::string JoinPath(const std::string& a, const std::string& b)
{
    if (a.empty()) return b;
    if (a.back() == '/' || a.back() == '\\') return a + b;
    return a + "/" + b;
}

std::string SaveManager::GetSaveFilePath(const std::string& filename)
{
    char* pref = SDL_GetPrefPath("rozmatix", "LiberteEgaliteRenaultCoupe");
    std::string path;
    if (pref)
    {
        path = JoinPath(pref, filename);
        SDL_free(pref);
    }
    else
    {
        path = filename;
    }
    return path;
}

bool SaveManager::Save(const SaveData& data)
{
    std::string path = GetSaveFilePath();

    std::ofstream ofs(path, std::ios::trunc);
    if (!ofs.is_open())
    {
        std::cerr << "SaveManager: failed to open " << path << " for writing\n";
        return false;
    }

    ofs << "SoundOn=" << (data.SoundOn ? "1" : "0") << "\n";
    ofs << "PlayerX=" << data.playerX << "\n";
    ofs << "PlayerY=" << data.playerY << "\n";
    ofs << "PlayerHP=" << data.playerHP << "\n";
    ofs << "GameTime=" << data.gameTime << "\n";

    // enemies
    for (size_t i = 0; i < data.enemies.size(); ++i)
    {
        const auto& e = data.enemies[i];
        ofs << "Enemy" << i << ".X=" << e.x << "\n";
        ofs << "Enemy" << i << ".Y=" << e.y << "\n";
        ofs << "Enemy" << i << ".HP=" << e.hp << "\n";
    }

    ofs.close();
    return true;
}

bool SaveManager::Load(SaveData& out)
{
    std::string path = GetSaveFilePath();
    std::ifstream ifs(path);
    if (!ifs.is_open())
    {
        return false;
    }

    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);

        if (key == "SoundOn")
        {
            out.SoundOn = (val == "1" || val == "true");
        }
        else if (key == "PlayerX")
        {
            out.playerX = static_cast<float>(std::atof(val.c_str()));
        }
        else if (key == "PlayerY")
        {
            out.playerY = static_cast<float>(std::atof(val.c_str()));
        }
        else if (key == "PlayerHP")
        {
            out.playerHP = std::atoi(val.c_str());
        }
        else if (key == "GameTime")
        {
            out.gameTime = static_cast<float>(std::atof(val.c_str()));
        }
        else if (key.rfind("Enemy", 0) == 0)
        {
            // format: Enemy{index}.{Field} e.g. Enemy0.X, Enemy1.HP
            size_t dotPos = key.find('.');
            if (dotPos == std::string::npos) continue;

            std::string idxStr = key.substr(5, dotPos - 5);
            int idx = std::atoi(idxStr.c_str());
            if (idx < 0) continue;

            std::string field = key.substr(dotPos + 1);

            if ((size_t)idx >= out.enemies.size())
                out.enemies.resize(idx + 1);

            if (field == "X")
            {
                out.enemies[idx].x = static_cast<float>(std::atof(val.c_str()));
            }
            else if (field == "Y")
            {
                out.enemies[idx].y = static_cast<float>(std::atof(val.c_str()));
            }
            else if (field == "HP")
            {
                out.enemies[idx].hp = std::atoi(val.c_str());
            }
        }
    }

    return true;
}