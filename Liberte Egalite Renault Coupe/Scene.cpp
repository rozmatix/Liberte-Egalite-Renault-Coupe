#include "Scene.h"
#include "SpriteBatch.h"
#include <SDL3/SDL.h>

void Scene::Update(float dt)
{
    if (player) player->Update(dt);
    for (auto& e : enemies) e.Update(dt);
}

void Scene::Draw(SpriteBatch* batch)
{
    if (!batch) return;

    // XP orbs first so enemies/player render over them
    for (const auto& orb : xpOrbs)
        batch->AddCircle(orb.position, orb.radius, SDL_Color{ 0, 200, 0, 255 });

    for (auto& e : enemies) e.Draw(batch);
    if (player) player->Draw(batch);
    for (auto& b : bullets) b.Draw(batch);

    // Draw torches (airborne or ground hazard)
    for (const auto& t : torches)
        t.Draw(batch);
}

