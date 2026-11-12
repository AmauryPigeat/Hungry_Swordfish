#include "screens.hpp"

#include <cmath>

namespace {

constexpr int hudMargin = 10;

// Sens demandé par le joueur : flèches ou ZQSD.
// Les scancodes désignent la position physique des touches : SDL_SCANCODE_W
// est la touche Z d'un clavier AZERTY, et la touche W d'un QWERTY.
Vec2 readDirection() {
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    Vec2 direction;
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) direction.x -= 1.0f;
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) direction.x += 1.0f;
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) direction.y -= 1.0f;
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) direction.y += 1.0f;
    return direction;
}

// 2.3 secondes restantes s'affichent "3" : on arrondit au-dessus.
int secondsToDisplay(float seconds) {
    return static_cast<int>(std::ceil(seconds));
}

}  // namespace

PlayingScreen::PlayingScreen(GameContext& context)
    : Screen(context),
      kills_(context.renderer, context.assets.mediumFont.get(), config::white),
      score_(context.renderer, context.assets.mediumFont.get(), config::white),
      timer_(context.renderer, context.assets.mediumFont.get(), config::white),
      countdown_(context.renderer, context.assets.largeFont.get(), config::red) {}

void PlayingScreen::handleEvent(const SDL_Event& event) {
    Screen::handleEvent(event);

    const bool escapePressed =
        event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE && event.key.repeat == 0;
    const bool focusLost =
        event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_FOCUS_LOST;
    if (escapePressed || focusLost) {
        goTo(ScreenId::Paused);
    }
}

void PlayingScreen::update(float dt) {
    GameSession& session = context_.session;
    session.update(dt, readDirection());

    if (session.isOver()) {
        context_.highScores.add({session.playerName(), session.kills(), session.score()});
        goTo(ScreenId::GameOver);
    }
}

void PlayingScreen::draw() {
    const GameSession& session = context_.session;
    session.draw(context_.renderer);

    kills_.set("Kills : " + std::to_string(session.kills()));
    score_.set("Score : " + std::to_string(session.score()));
    kills_.draw(hudMargin, hudMargin);
    score_.draw(hudMargin, hudMargin + kills_.height());

    if (session.inCountdown()) {
        dimScreen();
        countdown_.set(std::to_string(secondsToDisplay(session.countdown())));
        countdown_.drawCentered(screenCenterX, (config::windowHeight - countdown_.height()) / 2);
    } else {
        const int seconds = secondsToDisplay(session.timeLeft());
        timer_.set(std::to_string(seconds));
        timer_.setColor(seconds < config::lowTimeThreshold ? config::red : config::white);
        timer_.drawRightAligned(config::windowWidth - hudMargin, hudMargin);
    }
}
