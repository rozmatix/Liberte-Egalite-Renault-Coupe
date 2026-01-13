#pragma once
#include <vector>
#include "GuiLabel.h"

class GuiScene
{
public:
    void Draw(GuiRenderer& renderer);

    std::vector<GuiLabel*> labels;
};