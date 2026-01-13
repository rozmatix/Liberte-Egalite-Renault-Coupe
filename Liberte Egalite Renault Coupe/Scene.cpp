#include "Scene.h"
#include "Player.h"
#include "Enemy.h"
#include "SpriteBatch.h"

void Scene::Update(float dt)
{
    if (player)
        player->Update(dt);

    for (auto e : enemies)
        e.Update(dt);
}

void Scene::Draw(SpriteBatch* batch)
{
    if (player)
        player->Draw(batch);

    for (auto e : enemies)
        e.Draw(batch);
}

void Scene::AddAction(const std::function<void()>& action)
{
    actions.push_back(action);
}

void Scene::ExecuteActions()
{
    for (auto& a : actions)
        a();

    actions.clear();
}

