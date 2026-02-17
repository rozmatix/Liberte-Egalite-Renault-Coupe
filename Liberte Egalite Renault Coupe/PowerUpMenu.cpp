#include "PowerUpMenu.h"

void PowerUpMenu::Init(GuiScene& s)
{
    scene = &s;
    defs.clear();
    defs.push_back({ PowerUpType::Pierce,   "Pierce",   "+1 pierce" });
    defs.push_back({ PowerUpType::Fire,     "Fire",     "ignite enemies" });
    defs.push_back({ PowerUpType::Size,     "Size",     "Larger bullets" });
    defs.push_back({ PowerUpType::Multishot,"Multishot","bullets +1" });
    defs.push_back({ PowerUpType::Firerate, "Firerate", "+10% firerate" });
    defs.push_back({ PowerUpType::Speed,    "Speed",    "+10% car speed" });
    defs.push_back({ PowerUpType::ExtraHp,  "Extra HP", "+20 HP." });
    defs.push_back({ PowerUpType::HpRegen,  "HP Regen", "+5 HP regen" });
    defs.push_back({ PowerUpType::Frost,    "Frost",    "+10% slow" });
    defs.push_back({ PowerUpType::Power,    "Power",    "+5 damage" });

    const Vector2 btnSize{ 220.0f, 60.0f };
    for (int i = 0; i < 4; ++i)
    {
        nameLabels[i] = new GuiLabel("", Vector2(0,0));
        descLabels[i] = new GuiLabel("", Vector2(0,0));
        buttons[i]    = new GuiButton("Izberi", Vector2(0,0), btnSize);
        btnCallbacks[i] = [](){};
    }

    LayoutRow();
}

void PowerUpMenu::Open(GuiRenderer* guiRenderer)
{
    renderer = guiRenderer;
    if (!scene) return;

    open = true;
    selectionMade = false;
    ignoreInputTimer = IgnoreInputDuration;

    RandomizeOptions();

    for (int i = 0; i < 4; ++i)
    {
        const int defIndex = optionIndices[i];
        ApplyOptionToSlot(i, defIndex);

        if (defIndex >= 0 && defIndex < static_cast<int>(defs.size()))
        {
            const PowerUpType type = defs[defIndex].type;
            btnCallbacks[i] = [this, type]() {
                if (onChoose) onChoose(type);
                selectionMade = true;
                Close();
            };
        }
        else
        {
            btnCallbacks[i] = [this]() {
                selectionMade = true;
                Close();
            };
        }
    }
}

void PowerUpMenu::Close()
{
    open = false;
}

void PowerUpMenu::Draw()
{
    if (!open || !renderer) return;

    for (int i = 0; i < 4; ++i)
    {
        if (nameLabels[i]) nameLabels[i]->Draw(*renderer);
        if (descLabels[i]) descLabels[i]->Draw(*renderer);
        if (buttons[i])    buttons[i]->Draw(*renderer);
    }
}

void PowerUpMenu::Update(float dt)
{
    if (!open) return;
    if (ignoreInputTimer > 0.0f)
    {
        ignoreInputTimer = std::max(0.0f, ignoreInputTimer - dt);
    }
}

void PowerUpMenu::HandleClick(float mx, float my)
{
    if (!open) return;

    // Ignore player input until timer elapses
    if (ignoreInputTimer > 0.0f)
        return;

    for (int i = 0; i < 4; ++i)
    {
        GuiButton* b = buttons[i];
        if (!b) continue;
        const Vector2 pos = b->position;
        const Vector2 size = b->size;
        const bool inside =
            mx >= pos.x && mx <= (pos.x + size.x) &&
            my >= pos.y && my <= (pos.y + size.y);
        if (inside)
        {
            if (btnCallbacks[i]) btnCallbacks[i]();
            break;
        }
    }
}

void PowerUpMenu::LayoutRow()
{
    const float startX = 300.0f;
    const float spacingX = 260.0f;
    const float nameY = 280.0f;
    const float descY = nameY + 24.0f;
    const float btnY  = descY + 42.0f;

    for (int i = 0; i < 4; ++i)
    {
        const float x = startX + i * spacingX;
        if (nameLabels[i]) nameLabels[i]->position = Vector2(x, nameY);
        if (descLabels[i]) descLabels[i]->position = Vector2(x, descY);
        if (buttons[i])    buttons[i]->position    = Vector2(x, btnY);
    }
}

void PowerUpMenu::RandomizeOptions()
{
    for (int i = 0; i < 4; ++i) optionIndices[i] = -1;
    if (defs.empty()) return;

    std::vector<int> indices(defs.size());
    for (int i = 0; i < static_cast<int>(defs.size()); ++i) indices[i] = i;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(indices.begin(), indices.end(), rng);

    const int count = std::min(4, static_cast<int>(indices.size()));
    for (int i = 0; i < count; ++i)
        optionIndices[i] = indices[i];
}

void PowerUpMenu::ApplyOptionToSlot(int slot, int defIndex)
{
    auto wrap = [](const std::string& s, int maxCharsPerLine) {
        std::string out;
        int count = 0;
        for (size_t i = 0; i < s.size(); ++i)
        {
            char c = s[i];
            if (count >= maxCharsPerLine) { out.push_back('\n'); count = 0; }
            out.push_back(c);
            count++;
            if (c == ' ' && count >= maxCharsPerLine - 2) { out.push_back('\n'); count = 0; }
        }
        return out;
    };

    const int maxChars = 16;

    if (defIndex >= 0 && defIndex < static_cast<int>(defs.size()))
    {
        if (nameLabels[slot]) nameLabels[slot]->SetText(defs[defIndex].name);
        if (descLabels[slot]) descLabels[slot]->SetText(wrap(defs[defIndex].description, maxChars));
    }
    else
    {
        if (nameLabels[slot]) nameLabels[slot]->SetText("Empty");
        if (descLabels[slot]) descLabels[slot]->SetText(wrap("No power-up.", maxChars));
    }
}