#include "screens.hpp"

namespace {

constexpr int logoSize = 243;
constexpr int logoGap = 30;
constexpr int buttonSpacing = 15;

}  // namespace

WelcomeScreen::WelcomeScreen(GameContext& context) : Screen(context) {
    const int step = config::buttonHeight + buttonSpacing;
    const int firstButtonY = titleBottom(logoSize, logoGap) + buttonSpacing;

    addButton("START", buttonArea(screenCenterX, firstButtonY),
              [this] { goTo(ScreenId::NameEntry); });
    addButton("SCORE", buttonArea(screenCenterX, firstButtonY + step),
              [this] { goTo(ScreenId::Scores); });
    addButton("QUIT", buttonArea(screenCenterX, firstButtonY + 2 * step),
              [this] { goTo(ScreenId::Quit); });
}

void WelcomeScreen::draw() {
    drawMenuBackground();
    drawLogo(logoSize, logoGap);
    drawButtons();
}
