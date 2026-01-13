#pragma once

class Gameplay;

class GameState
{
public:
    virtual ~GameState() {}
    virtual void HandleInput(bool keyState[]) = 0;
    virtual void Update(float dt) = 0;
    virtual void Draw() = 0;

    virtual bool IsActive() const { return true; }
};