#include "Gameplay.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "nlohmann/json.hpp"
#include "SoundManager.h"
#include "Bullet.h"
#include "SaveManager.h"
#include "Enemy.h"
using json = nlohmann::json;

bool Gameplay::Init()
{
    if (!renderer.Init("SDL3 Game", 1500, 1000))
        return false;

    if (!SoundManager::Instance().Init())
    {
        std::cerr << "SoundManager failed to initialize\n";
        return false;
    }

    SoundManager::Instance().LoadWav("crash", "car-crash-sound.wav");
    SoundManager::Instance().LoadWav("engine", "engine.wav");

    SDL_Texture* texture = IMG_LoadTexture(renderer.renderer, "texture.png");
    if (!texture)
    {
        std::cerr << "Failed to load texture.png" << std::endl;
        return false;
    }

    LoadSpritesFromJSON("texture.json", texture);

    if (!sprites.empty())
    {
        scene.player = new Player();
        scene.player->Init(sprites[1]);
        scene.player->position = Vector2(500.0f, 500.0f);
        scene.player->SetGunSprite(sprites.size() > 4 ? sprites[4] : nullptr);
        scene.player->SetGunOffsetPercent(-0.25f, 0);
        scene.player->SetFireRate(0.8f);

        {
            Enemy farmerNone(EnemyType::Farmer, Vector2(800.0f, 1200.0f));
            farmerNone.Init(sprites.size() > 0 ? sprites[0] : nullptr);
            farmerNone.SetWeapon(std::nullopt);
            farmerNone.SetWeaponSprite(nullptr);
            scene.enemies.push_back(farmerNone);
        }
    }
    else
    {
        std::cerr << "ERROR: No sprites loaded, cannot initialize player!" << std::endl;
        return false;
    }

    if (!guiRenderer.Init(renderer.renderer))
    {
        std::cerr << "GuiRenderer failed to load" << std::endl;
        return false;
    }

    physics.onEnemyDamaged = [this](const Vector2& worldPos, int dmg)
        {
            const Vector2 screenPos = renderer.spriteBatch->WorldToScreen(worldPos);
            GuiLabel* lbl = new GuiLabel("-" + std::to_string(dmg), screenPos);
            guiScene.labels.push_back(lbl);

            damagePopups.push_back(DamagePopup{ lbl, 0.8f, worldPos });
        };

    renderer.spriteBatch->SetCameraScale(1.0f / 3.0f);
    physics.SetWorldBounds(0.0f, 0.0f, 4000.0f, 3000.0f);

    InitSpawners();

    hpLabel = new GuiLabel("HP: 100", Vector2(20, 20));
    guiScene.labels.push_back(hpLabel);
    timeLabel = new GuiLabel("Time: 0.0", Vector2(1250, 20));
    guiScene.labels.push_back(timeLabel);

    levelLabel = new GuiLabel("LVL: 1", Vector2(20, 60));
    guiScene.labels.push_back(levelLabel);
    xpLabel = new GuiLabel("XP: 0 / 50", Vector2(20, 90));
    guiScene.labels.push_back(xpLabel);
    lastSeenPlayerLevel = scene.player ? scene.player->GetLevel() : 1;

    escWasDown = false;
    cWasDown = false;

    powerMenu.onChoose = [this](PowerUpType type)
        {
            switch (type)
            {
            case PowerUpType::Pierce:    playerPierceExtra += 1; break;
            case PowerUpType::Fire:      playerFireLevel += 1; break;
            case PowerUpType::Size:      playerBulletSizeLevel += 1; break;
            case PowerUpType::Multishot: playerMultishotLevel += 1; break;
            case PowerUpType::Firerate:
                playerFireRateLevel += 1;
                if (scene.player) scene.player->SetFireRate(scene.player->GetFireRate() * 0.9f);
                break;
            case PowerUpType::Speed:
                playerMaxSpeedLevel += 1;
                if (scene.player) scene.player->SetMaxSpeedMultiplier(1.1f);
                break;
            case PowerUpType::ExtraHp:
                playerExtraHpStacks += 1;
                if (scene.player) { scene.player->IncreaseMaxHp(20); scene.player->Heal(20); }
                break;
            case PowerUpType::HpRegen:   playerHpRegenLevel += 1; break;
            case PowerUpType::Frost:     playerFrostLevel += 1; break;
            case PowerUpType::Power:     playerPowerLevel += 1; break;
            }
            ClosePowerMenu();
        };

    gameOverRequested = false;
    gameEnded = false;

    return true;
}

void Gameplay::LoadSpritesFromJSON(const std::string& jsonFile, SDL_Texture* texture)
{
    std::ifstream iFile(jsonFile);
    if (!iFile.is_open())
    {
        std::cerr << "Failed to open " << jsonFile << std::endl;
        return;
    }

    json j;
    try { iFile >> j; }
    catch (json::parse_error& e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return;
    }

    if (!j.contains("textures") || !j["textures"].is_array()) return;

    for (auto& textureEntry : j["textures"])
    {
        if (!textureEntry.contains("frames") || !textureEntry["frames"].is_array()) continue;

        for (auto& frame : textureEntry["frames"])
        {
            if (!frame.contains("frame")) continue;

            auto f = frame["frame"];
            float w = f.value("w", 0.0f);
            float h = f.value("h", 0.0f);
            if (w <= 0 || h <= 0) continue;

            SDL_FRect rect;
            rect.x = f.value("x", 0.0f);
            rect.y = f.value("y", 0.0f);
            rect.w = w;
            rect.h = h;

            std::string name = frame.value("filename", "unknown");
            if (!texture) continue;

            Sprite* s = new Sprite(name, texture, rect);
            sprites.push_back(s);
        }
    }
}

void Gameplay::SpawnWaveLikeInit()
{
    if (sprites.empty()) return;

    Enemy farmerTorch(EnemyType::Farmer, Vector2(1800.0f, 1400.0f));
    farmerTorch.Init(sprites.size() > 0 ? sprites[0] : nullptr);
    farmerTorch.SetWeapon(WeaponType::Torch);
    farmerTorch.SetWeaponSprite(sprites.size() > 3 ? sprites[3] : nullptr);
    scene.enemies.push_back(farmerTorch);
}

void Gameplay::CheckRules()
{
    scene.enemies.erase(
        std::remove_if(scene.enemies.begin(), scene.enemies.end(),
            [](const Enemy& e) { return e.current_hp <= 0; }),
        scene.enemies.end());
}

void Gameplay::InitSpawners()
{
    spawners.clear();

    const float W = 4000.0f, H = 3000.0f;
    const Vector2 corners[4] = { Vector2(0,0), Vector2(W,0), Vector2(W,H), Vector2(0,H) };
    const Vector2 mids[4] = { Vector2(W * 0.5f,0), Vector2(W, H * 0.5f), Vector2(W * 0.5f,H), Vector2(0, H * 0.5f) };

    for (const auto& p : corners) spawners.emplace_back(EnemySpawner(p));
    for (const auto& p : mids)    spawners.emplace_back(EnemySpawner(p));

    for (size_t i = 0; i < spawners.size(); ++i)
        spawners[i].SetActive(i < 2);
}

void Gameplay::UpdateSpawners(float dt)
{
    const float remainingSec = std::max(0.0f, totalGameLengthSec - gameTime);

    const float startRemain = totalGameLengthSec;
    const float fullRemain = 20.0f * 60.0f;

    float t = 0.0f;
    if (remainingSec > fullRemain)
    {
        t = (startRemain - remainingSec) / (startRemain - fullRemain);
        t = std::max(0.0f, std::min(1.0f, t));
    }
    else
    {
        t = 1.0f;
    }

    const int minActive = 2;
    const int maxActive = static_cast<int>(spawners.size());
    const int shouldActive = minActive + static_cast<int>(std::round(t * (maxActive - minActive)));

    for (size_t i = 0; i < spawners.size(); ++i)
        spawners[i].SetActive(static_cast<int>(i) < shouldActive);

    const size_t before = scene.enemies.size();

    for (auto& s : spawners)
        s.Update(dt, remainingSec, totalGameLengthSec, scene);

    for (size_t i = before; i < scene.enemies.size(); ++i)
    {
        Enemy& e = scene.enemies[i];

        if (e.GetType() == EnemyType::Farmer)
        {
            e.Init(sprites.size() > 0 ? sprites[0] : nullptr);
            const WeaponType w = e.GetWeapon();
            if (w == WeaponType::Torch)
                e.SetWeaponSprite(sprites.size() > 3 ? sprites[3] : nullptr);
            else if (w == WeaponType::Pitchfork)
                e.SetWeaponSprite(sprites.size() > 5 ? sprites[5] : nullptr);
            else
                e.SetWeaponSprite(nullptr);
        }
        else
        {
            e.Init(sprites.size() > 2 ? sprites[2] : nullptr);
            if (e.GetWeapon() == WeaponType::Rifle)
                e.SetWeaponSprite(sprites.size() > 4 ? sprites[4] : nullptr);
            else
                e.SetWeaponSprite(nullptr);
        }
    }
}

void Gameplay::UpdateCountdownHud(float remainingSec)
{
    int rem = static_cast<int>(std::round(remainingSec));
    int mm = rem / 60;
    int ss = rem % 60;
    if (timeLabel)
    {
        char buf[32];
        std::snprintf(buf, sizeof(buf), "Time: %02d:%02d", mm, ss);
        timeLabel->SetText(buf);
    }

    if (remainingSec <= 0.0f)
    {
        gameOverRequested = true;
        return;
    }
}

void Gameplay::Update(float dt, bool keyState[])
{
    if (gameOverRequested)
        return;

    if (!scene.player) return;

    const bool escDown = keyState[SDL_SCANCODE_ESCAPE] != 0;
    if (escDown && !escWasDown)
    {
        if (!paused && !powerMenuOpen)
        {
            pauseMenu.Open(&guiRenderer);
            paused = true;
        }
        else if (paused && !powerMenuOpen)
        {
            pauseMenu.Close();
            paused = false;
        }
    }
    escWasDown = escDown;

    if (powerMenuOpen)
    {
        powerMenu.Update(dt);

        float mx = 0.0f, my = 0.0f;
        const SDL_MouseButtonFlags mb = SDL_GetMouseState(&mx, &my);
        if (mb & SDL_BUTTON_LMASK) powerMenu.HandleClick(mx, my);

        if (!powerMenu.IsOpen())
        {
            powerMenuOpen = false;
            paused = false;
        }
        return;
    }

    if (paused)
    {
        float mx = 0.0f, my = 0.0f;
        const SDL_MouseButtonFlags mb = SDL_GetMouseState(&mx, &my);
        if (mb & SDL_BUTTON_LMASK) pauseMenu.HandleClick(mx, my);

        if (pauseMenu.resumeRequested)
        {
            pauseMenu.Close();
            paused = false;
        }
        if (pauseMenu.quitRequested)
        {
            SoundManager::Instance().Shutdown();
            SDL_Quit();
            std::exit(0);
        }
        return;
    }

    const bool cDown = keyState[SDL_SCANCODE_C] != 0;
    cWasDown = cDown;

    scene.player->HandleInput(keyState, dt);

    for (auto& e : scene.enemies)
    {
        e.AI(scene, dt);
        e.Update(dt);
    }

    float mx = 0.0f, my = 0.0f;
    const SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mx, &my);
    if ((mouseButtons & SDL_BUTTON_LMASK) && scene.player->CanFire())
    {
        const Vector2 mouseWorld = renderer.spriteBatch->ScreenToWorld(mx, my);
        const Vector2 muzzle = scene.player->GetGunMuzzleWorld(mouseWorld);
        Vector2 toMouse{ mouseWorld.x - muzzle.x, mouseWorld.y - muzzle.y };
        float lenSq = toMouse.x * toMouse.x + toMouse.y * toMouse.y;
        if (lenSq > 0.0001f)
        {
            float len = std::sqrt(lenSq);
            Vector2 dir{ toMouse.x / len, toMouse.y / len };

            Bullet b;
            b.Init(muzzle, dir, false, playerPierceExtra);
            b.fireLevel = playerFireLevel;
            b.frostLevel = playerFrostLevel;
            b.powerBonus = playerPowerLevel;
            b.radius += playerBulletSizeLevel * 2;
            scene.bullets.push_back(b);

            if (playerMultishotLevel > 0)
            {
                const float baseAngle = std::atan2(dir.y, dir.x);
                const float degToRad = 3.14159265358979323846f / 180.0f;
                for (int i = 0; i < playerMultishotLevel; ++i)
                {
                    const float offsetDeg = 6.0f + i * 4.0f;
                    for (int side = -1; side <= 1; side += 2)
                    {
                        const float ang = baseAngle + side * (offsetDeg * degToRad);
                        const Vector2 d{ std::cos(ang), std::sin(ang) };

                        Bullet bx;
                        bx.Init(muzzle, d, false, playerPierceExtra);
                        bx.fireLevel = playerFireLevel;
                        bx.frostLevel = playerFrostLevel;
                        bx.powerBonus = playerPowerLevel;
                        bx.radius += playerBulletSizeLevel * 2;
                        scene.bullets.push_back(bx);
                    }
                }
            }

            scene.player->ConsumeFire();
        }
    }

    if (scene.player)
    {
        const Vector2 pSize = scene.player->HitboxSize();
        scene.xpOrbs.erase(
            std::remove_if(scene.xpOrbs.begin(), scene.xpOrbs.end(),
                [&](const XpOrb& orb)
                {
                    const float dx = orb.position.x - scene.player->position.x;
                    const float dy = orb.position.y - scene.player->position.y;
                    const float distSq = dx * dx + dy * dy;
                    const float pickRadius = std::max(pSize.x, pSize.y) * 0.5f;
                    if (distSq <= (orb.radius + pickRadius) * (orb.radius + pickRadius))
                    {
                        scene.player->GainXp(orb.xpAmount);
                        return true;
                    }
                    return false;
                }),
            scene.xpOrbs.end());
    }

    const bool moving =
        keyState[SDL_SCANCODE_W] ||
        keyState[SDL_SCANCODE_A] ||
        keyState[SDL_SCANCODE_S] ||
        keyState[SDL_SCANCODE_D];

    if (moving) SoundManager::Instance().StartLoop("engine");
    else SoundManager::Instance().StopLoop("engine");
    SoundManager::Instance().Update();

    physics.Simulate(scene, dt);

    const Vector2 screenCenter{ 1500.0f * 0.5f, 1000.0f * 0.5f };
    renderer.spriteBatch->SetCameraScale(1.0f / 3.0f);
    renderer.spriteBatch->SetCameraFocus(scene.player->position, screenCenter);

    scene.Update(dt);
    CheckRules();

    for (auto& dp : damagePopups)
    {
        dp.ttl = std::max(0.0f, dp.ttl - dt);
        if (dp.label)
        {
            const Vector2 screenPos = renderer.spriteBatch->WorldToScreen(dp.worldPos);
            dp.label->position = screenPos;
        }
    }

    damagePopups.erase(
        std::remove_if(damagePopups.begin(), damagePopups.end(),
            [this](DamagePopup& dp)
            {
                if (dp.ttl <= 0.0f && dp.label)
                {
                    auto it = std::find(guiScene.labels.begin(), guiScene.labels.end(), dp.label);
                    if (it != guiScene.labels.end()) guiScene.labels.erase(it);
                    delete dp.label;
                    dp.label = nullptr;
                    return true;
                }
                return false;
            }),
        damagePopups.end());

    if (hpLabel)
        hpLabel->SetText("HP: " + std::to_string(scene.player->current_hp));
    if (timeLabel)
        timeLabel->SetText("Time: " + std::to_string((int)gameTime));

    const float remainingSec = std::max(0.0f, totalGameLengthSec - gameTime);
    UpdateCountdownHud(remainingSec);

    if (scene.player)
    {
        if (levelLabel) levelLabel->SetText("LVL: " + std::to_string(scene.player->GetLevel()));
        if (xpLabel)    xpLabel->SetText("XP: " + std::to_string(scene.player->GetXp()) + " / " + std::to_string(scene.player->GetXpRequired()));
    }

    if (scene.player && scene.player->GetLevel() > lastSeenPlayerLevel && !powerMenuOpen)
    {
        lastSeenPlayerLevel = scene.player->GetLevel();
        OpenPowerMenu();
        return;
    }

    if (scene.player && playerHpRegenLevel > 0)
    {
        hpRegenTimer += dt;
        const float interval = 20.0f;
        while (hpRegenTimer >= interval)
        {
            hpRegenTimer -= interval;
            const int heal = 5 * playerHpRegenLevel;
            scene.player->Heal(heal);
        }
    }

    gameTime += dt;
    UpdateSpawners(dt);

    if (scene.player && scene.player->current_hp <= 0)
    {
        gameOverRequested = true;
        return;
    }
}

void Gameplay::Draw()
{
    renderer.Clear();

    const Vector2 screenCenter{ 1500.0f * 0.5f, 1000.0f * 0.5f };
    renderer.spriteBatch->SetCameraScale(1.0f / 3.0f);
    renderer.spriteBatch->SetCameraFocus(scene.player->position, screenCenter);

    scene.Draw(renderer.spriteBatch);

    const float worldX = 0.0f, worldY = 0.0f, worldW = 4000.0f, worldH = 3000.0f;
    const Vector2 tl = renderer.spriteBatch->WorldToScreen(Vector2{ worldX, worldY });
    const Vector2 tr = renderer.spriteBatch->WorldToScreen(Vector2{ worldX + worldW, worldY });
    const Vector2 br = renderer.spriteBatch->WorldToScreen(Vector2{ worldX + worldW, worldY + worldH });
    const Vector2 bl = renderer.spriteBatch->WorldToScreen(Vector2{ worldX, worldY + worldH });

    SDL_FPoint pts[5] = { { tl.x, tl.y }, { tr.x, tr.y }, { br.x, br.y }, { bl.x, bl.y }, { tl.x, tl.y } };
    SDL_SetRenderDrawColor(renderer.renderer, 0, 0, 0, 255);
    SDL_RenderLines(renderer.renderer, pts, 5);

    guiScene.Draw(guiRenderer);

    renderer.Present();
}

void Gameplay::OpenPowerMenu()
{
    if (!powerMenu.IsOpen())
    {
        powerMenu.Init(guiScene);
    }

    powerMenu.Open(&guiRenderer);
    powerMenuOpen = true;
    paused = true;
}

void Gameplay::ClosePowerMenu()
{
    powerMenu.Close();
    powerMenuOpen = false;
    paused = false;
}

// FillSaveData / LoadFromSave ostaneta nespremenjena ...

// Save current gameplay state
void Gameplay::FillSaveData(SaveData& out) const
{
    out.SoundOn = true;

    if (scene.player)
    {
        out.playerX = scene.player->position.x;
        out.playerY = scene.player->position.y;
        out.playerHP = scene.player->current_hp;
        out.playerAngle = scene.player->AngleDeg();
    }
    else
    {
        out.playerX = 0.0f;
        out.playerY = 0.0f;
        out.playerHP = 0;
        out.playerAngle = -90.0f;
    }

    out.gameTime = gameTime;

    // --- Enemies ---
    out.enemies.clear();
    out.enemies.reserve(scene.enemies.size());
    for (const auto& e : scene.enemies)
    {
        EnemySave es;
        es.x = e.position.x;
        es.y = e.position.y;
        es.hp = e.current_hp;
        es.type = static_cast<int>(e.GetType());
        es.weapon = static_cast<int>(e.GetWeapon());
        es.angle = e.AngleDeg();
        out.enemies.push_back(es);
    }

    // --- Bullets ---
    out.bullets.clear();
    out.bullets.reserve(scene.bullets.size());
    for (const auto& b : scene.bullets)
    {
        BulletSave bs;
        bs.x = b.position.x;
        bs.y = b.position.y;
        bs.dx = b.velocity.x;
        bs.dy = b.velocity.y;
        out.bullets.push_back(bs);
    }

    // --- Torches ---
    out.torches.clear();
    out.torches.reserve(scene.torches.size());
    for (const auto& t : scene.torches)
    {
        TorchSave ts;
        ts.x = t.position.x;
        ts.y = t.position.y;
        ts.lit = !t.airborne; // približek: če ni v zraku, je "prižgana" na tleh
        out.torches.push_back(ts);
    }

    // --- XP orbs ---
    out.xpOrbs.clear();
    out.xpOrbs.reserve(scene.xpOrbs.size());
    for (const auto& xp : scene.xpOrbs)
    {
        XpOrbSave xs;
        xs.x = xp.position.x;
        xs.y = xp.position.y;
        xs.xpAmount = xp.xpAmount;
        out.xpOrbs.push_back(xs);
    }
}

// Load gameplay state
void Gameplay::LoadFromSave(const SaveData& in)
{
    // Inicializiraj igralca, če ga ni
    if (!scene.player)
        scene.player = new Player();

    // Nastavi igralčeve podatke
    scene.player->position.x = in.playerX;
    scene.player->position.y = in.playerY;
    scene.player->angle = in.playerAngle;
    scene.player->current_hp = in.playerHP > 0 ? in.playerHP : 100;

    // Nastavi čas igre
    gameTime = in.gameTime;

    // --- Enemies ---
    scene.enemies.clear();
    for (const auto& e : in.enemies)
    {
        Enemy enemy(static_cast<EnemyType>(e.type), Vector2(e.x, e.y));
        enemy.current_hp = e.hp;
        enemy.SetWeapon(static_cast<WeaponType>(e.weapon));
        enemy.angle = e.angle;

        if (enemy.GetType() == EnemyType::Farmer)
        {
            enemy.Init(sprites.size() > 0 ? sprites[0] : nullptr);
            if (enemy.GetWeapon() == WeaponType::Torch)
                enemy.SetWeaponSprite(sprites.size() > 3 ? sprites[3] : nullptr);
            else if (enemy.GetWeapon() == WeaponType::Pitchfork)
                enemy.SetWeaponSprite(sprites.size() > 5 ? sprites[5] : nullptr);
            else
                enemy.SetWeaponSprite(nullptr);
        }
        else
        {
            enemy.Init(sprites.size() > 2 ? sprites[2] : nullptr);
            if (enemy.GetWeapon() == WeaponType::Rifle)
                enemy.SetWeaponSprite(sprites.size() > 4 ? sprites[4] : nullptr);
            else
                enemy.SetWeaponSprite(nullptr);
        }

        scene.enemies.push_back(enemy);
    }

    // --- Bullets ---
    scene.bullets.clear();
    for (const auto& b : in.bullets)
    {
        Vector2 dir{ b.dx, b.dy };
        float lenSq = dir.x * dir.x + dir.y * dir.y;
        if (lenSq < 0.0001f)
            continue; // ignoriraj slabe zapise

        float len = std::sqrt(lenSq);
        dir.x /= len;
        dir.y /= len;

        Bullet bullet;
        bullet.Init(Vector2(b.x, b.y), dir, /*fromEnemyBullet*/ false, /*pierceExtra*/ playerPierceExtra);
        // če želiš, lahko tukaj nastaviš še fireLevel/frostLevel/powerBonus/radius iz dodatnih SaveData polj
        scene.bullets.push_back(bullet);
    }

    // --- Torches ---
    scene.torches.clear();
    for (const auto& t : in.torches)
    {
        Torch torch;
        // Zaenkrat postavimo na tla na poziciji; brez leta, samo hazard
        torch.sprite = sprites.size() > 3 ? sprites[3] : nullptr;
        torch.position = Vector2(t.x, t.y);
        torch.velocity = Vector2(0.0f, 0.0f);
        torch.airborne = false;
        torch.dead = !t.lit;  // če ni "lit", ga lahko takoj ignoriramo
        scene.torches.push_back(torch);
    }

    // --- XP orbs ---
    scene.xpOrbs.clear();
    for (const auto& x : in.xpOrbs)
    {
        XpOrb orb;
        orb.position = Vector2(x.x, x.y);
        orb.radius = 14.0f;
        orb.xpAmount = x.xpAmount;
        scene.xpOrbs.push_back(orb);
    }

    // Ponastavi menije in stanje igre
    pauseMenu.Close();
    paused = false;
    powerMenuOpen = false;
}

