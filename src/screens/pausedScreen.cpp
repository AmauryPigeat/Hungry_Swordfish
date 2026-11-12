#include "screens.hpp"

namespace {

constexpr int logoSize = 243;
constexpr int logoGap = 30;

}  // namespace

PausedScreen::PausedScreen(GameContext& context)
    : Screen(context),
      message_(context.renderer, context.assets.mediumFont.get(), config::mint,
               "THE GAME IS PAUSED") {
    const std::vector<SDL_Rect> areas = buttonRow(3, titleBottom(logoSize, logoGap) + 250);
    addButton("RESUME", areas[0], [this] { goTo(ScreenId::Playing); });
    addButton("HOME", areas[1], [this] { goTo(ScreenId::Welcome); });
    addButton("QUIT", areas[2], [this] { goTo(ScreenId::Quit); });
}

void PausedScreen::handleEvent(const SDL_Event& event) {
    Screen::handleEvent(event);

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE &&
        event.key.repeat == 0) {
        goTo(ScreenId::Playing);
    }
}

void PausedScreen::draw() {
    // La partie n'est plus mise à jour, mais on la dessine toujours en arrière-plan.
    context_.session.draw(context_.renderer);
    dimScreen();

    drawLogo(logoSize, logoGap);
    message_.drawCentered(screenCenterX, titleBottom(logoSize, logoGap) + 60);
    drawButtons();
}
