#include "GuiLabel.h"
#include "GuiRenderer.h"

GuiLabel::GuiLabel(const std::string& txt, Vector2 pos)
    : text(txt), position(pos) {
}

void GuiLabel::SetText(const std::string& txt)
{
    text = txt;
}

void GuiLabel::Draw(GuiRenderer& renderer)
{
    renderer.DrawText(text, position);
}