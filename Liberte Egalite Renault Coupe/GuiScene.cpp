#include "GuiScene.h"
#include "GuiRenderer.h"

void GuiScene::Draw(GuiRenderer& renderer)
{
    for (auto* label : labels)
        label->Draw(renderer);
}