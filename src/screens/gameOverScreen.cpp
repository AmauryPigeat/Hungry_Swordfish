#include "screens.hpp"

namespace {

constexpr int logoSize = 243;
constexpr int logoGap = 30;
constexpr int lineSpacing = 50;

}  // namespace

GameOverScreen::GameOverScreen(GameContext& context)
    : Screen(context),
      score_(context.renderer, context.assets.mediumFont.get(), config::white),
      kills_(context.renderer, context.assets.mediumFont.get(), config::white) {
    const std::vector<SDL_Rect> areas = buttonRow(3, titleBottom(logoSize, logoGap) + 250);

    // On rejoue directement avec le même pseudo.
    addButton("RE-START", areas[0], [this] {
        context_.session.start(context_.session.playerName());
        goTo(ScreenId::Playing);
    });
    addButton("HOME", areas[1], [this] { goTo(ScreenId::Welcome); });
    addButton("QUIT", areas[2], [this] { goTo(ScreenId::Quit); });
}

void GameOverScreen::onEnter() {
    score_.set("Your final score : " + std::to_string(context_.session.score()));
    kills_.set("Your fish kills : " + std::to_string(context_.session.kills()));
}

void GameOverScreen::draw() {
    drawMenuBackground();
    drawLogo(logoSize, logoGap);

    const int scoreY = titleBottom(logoSize, logoGap) + lineSpacing;
    score_.drawCentered(screenCenterX, scoreY);
    kills_.drawCentered(screenCenterX, scoreY + score_.height() + lineSpacing);
    drawButtons();
}
