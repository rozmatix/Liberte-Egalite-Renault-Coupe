#include "SaveManager.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

bool SaveManager::Save(const SaveData& data)
{
    json j;
    j["SoundOn"] = data.SoundOn;
    j["PlayerX"] = data.playerX;
    j["PlayerY"] = data.playerY;
    j["PlayerHP"] = data.playerHP;
    j["PlayerAngle"] = data.playerAngle; // new
    j["GameTime"] = data.gameTime;

    j["Enemies"] = json::array();
    for (const auto& e : data.enemies)
    {
        j["Enemies"].push_back({
            {"x", e.x},
            {"y", e.y},
            {"hp", e.hp},
            {"type", e.type},
            {"weapon", e.weapon},
            {"angle", e.angle}
            });
    }

    j["Bullets"] = json::array();
    for (const auto& b : data.bullets)
    {
        j["Bullets"].push_back({ {"x", b.x}, {"y", b.y}, {"dx", b.dx}, {"dy", b.dy} });
    }

    j["Torches"] = json::array();
    for (const auto& t : data.torches)
    {
        j["Torches"].push_back({ {"x", t.x}, {"y", t.y}, {"lit", t.lit} });
    }

    j["XpOrbs"] = json::array();
    for (const auto& xp : data.xpOrbs)
    {
        j["XpOrbs"].push_back({ {"x", xp.x}, {"y", xp.y}, {"xpAmount", xp.xpAmount} });
    }

    std::ofstream ofs("save.json");
    if (!ofs.is_open()) return false;
    ofs << j.dump(2);
    return true;
}

bool SaveManager::Load(SaveData& out)
{
    std::ifstream ifs("save.json");
    if (!ifs.is_open()) return false;

    json j;
    ifs >> j;

    out.SoundOn = j.value("SoundOn", true);
    out.playerX = j.value("PlayerX", 0.0f);
    out.playerY = j.value("PlayerY", 0.0f);
    out.playerHP = j.value("PlayerHP", 0);
    out.playerAngle = j.value("PlayerAngle", -90.0f); // new
    out.gameTime = j.value("GameTime", 0.0f);

    out.enemies.clear();
    if (j.contains("Enemies") && j["Enemies"].is_array())
    {
        for (auto& je : j["Enemies"])
        {
            EnemySave es;
            es.x = je.value("x", 0.0f);
            es.y = je.value("y", 0.0f);
            es.hp = je.value("hp", 0);
            es.type = je.value("type", 0);
            es.weapon = je.value("weapon", 0);
            es.angle = je.value("angle", 0.0f);
            out.enemies.push_back(es);
        }
    }

    out.bullets.clear();
    if (j.contains("Bullets") && j["Bullets"].is_array())
    {
        for (auto& jb : j["Bullets"])
        {
            BulletSave bs;
            bs.x = jb.value("x", 0.0f);
            bs.y = jb.value("y", 0.0f);
            bs.dx = jb.value("dx", 0.0f);
            bs.dy = jb.value("dy", 0.0f);
            out.bullets.push_back(bs);
        }
    }

    out.torches.clear();
    if (j.contains("Torches") && j["Torches"].is_array())
    {
        for (auto& jt : j["Torches"])
        {
            TorchSave ts;
            ts.x = jt.value("x", 0.0f);
            ts.y = jt.value("y", 0.0f);
            ts.lit = jt.value("lit", false);
            out.torches.push_back(ts);
        }
    }

    out.xpOrbs.clear();
    if (j.contains("XpOrbs") && j["XpOrbs"].is_array())
    {
        for (auto& jxp : j["XpOrbs"])
        {
            XpOrbSave xs;
            xs.x = jxp.value("x", 0.0f);
            xs.y = jxp.value("y", 0.0f);
            xs.xpAmount = jxp.value("xpAmount", 0);
            out.xpOrbs.push_back(xs);
        }
    }

    return true;
}