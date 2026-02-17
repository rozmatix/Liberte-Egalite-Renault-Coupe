#pragma once
#include <vector>
#include <functional>
#include <random>
#include <string>
#include "GuiScene.h"
#include "GuiButton.h"
#include "GuiLabel.h"
#include "GuiRenderer.h"
#include "Vector2.h"

enum class PowerUpType
{
    Pierce,
    Fire,
    Size,
    Multishot,
    Firerate,
    Speed,
    ExtraHp,
    HpRegen,
    Frost,
    Power
};

// Definition for power-up entries used by the menu
struct PowerUpDef
{
    PowerUpType type;
    std::string name;
    std::string description;
};

class PowerUpMenu
{
public:
    bool IsOpen() const { return open; }
    void Open(GuiRenderer* guiRenderer);
    void Close();
    void Draw();
    void HandleClick(float mx, float my);
    void Update(float dt); // new: tick ignore timer

    std::function<void(PowerUpType)> onChoose;

    bool open = false;
    bool selectionMade = false;

    GuiScene* scene = nullptr;
    GuiRenderer* renderer = nullptr;

    void Init(GuiScene& s);

private:
    GuiButton* buttons[4] { nullptr, nullptr, nullptr, nullptr };
    GuiLabel*  nameLabels[4] { nullptr, nullptr, nullptr, nullptr };
    GuiLabel*  descLabels[4] { nullptr, nullptr, nullptr, nullptr };
    std::function<void()> btnCallbacks[4];
    int optionIndices[4] { -1, -1, -1, -1 };
    std::vector<PowerUpDef> defs;

    // Timed input gating (first 3s ignore)
    float ignoreInputTimer = 0.0f;
    static constexpr float IgnoreInputDuration = 3.0f;

    void LayoutRow();
    void RandomizeOptions();
    void ApplyOptionToSlot(int slot, int defIndex);
};  