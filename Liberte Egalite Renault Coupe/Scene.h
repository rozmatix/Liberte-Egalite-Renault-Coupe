#pragma once
#include <vector>
#include <functional>

class Player;
class Enemy;

class Scene
{
public:
    void Update(float dt);
    void Draw(class SpriteBatch* batch);

    // akcije (dodaj / odstrani po koncu frame-a)
    void AddAction(const std::function<void()>& action);
    void ExecuteActions();

    Player* player = nullptr;
    std::vector<Enemy> enemies;

private:
    std::vector<std::function<void()>> actions;
};