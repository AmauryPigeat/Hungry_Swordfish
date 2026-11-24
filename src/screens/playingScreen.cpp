#include "screens.hpp"

#include <cmath>

namespace {

constexpr int hudMargin = 10;
constexpr int dashGaugeWidth = 120;
constexpr int dashGaugeHeight = 12;

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
      countdown_(context.renderer, context.assets.largeFont.get(), config::red),
      sprintEffect_(context.renderer, context.assets.smallFont.get(), config::sprintBonusColor),
      doubleScoreEffect_(context.renderer, context.assets.smallFont.get(),
                         config::scoreBonusColor),
      dashLabel_(context.renderer, context.assets.smallFont.get(), config::white, "DASH") {}

void PlayingScreen::handleEvent(const SDL_Event& event) {
    Screen::handleEvent(event);

    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_SPACE &&
        event.key.repeat == 0) {
        dashPressed_ = true;
    }

    // Molette vers le haut : zoom, vers le bas : dézoom.
    if (event.type == SDL_MOUSEWHEEL) {
        int steps = event.wheel.y;
        // Certains systèmes (souris "naturelles", macOS) inversent le sens de la molette.
        if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
            steps = -steps;
        }
        context_.session.zoomWithWheel(steps);
    }

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
    // L'appui est consommé par ce tick : il ne déclenche qu'un seul dash,
    // et il est perdu s'il tombe pendant le décompte.
    session.update(dt, {readDirection(), dashPressed_});
    dashPressed_ = false;

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

    // Effets de bonus actifs, sous le score.
    int effectY = hudMargin + kills_.height() + score_.height();
    if (session.sprintTimeLeft() > 0.0f) {
        sprintEffect_.set("SPRINT " + std::to_string(secondsToDisplay(session.sprintTimeLeft())));
        sprintEffect_.draw(hudMargin, effectY);
        effectY += sprintEffect_.height();
    }
    if (session.doubleScoreTimeLeft() > 0.0f) {
        doubleScoreEffect_.set("SCORE x" + std::to_string(config::scoreMultiplier) + " " +
                               std::to_string(secondsToDisplay(session.doubleScoreTimeLeft())));
        doubleScoreEffect_.draw(hudMargin, effectY);
    }

    if (session.inCountdown()) {
        dimScreen();
        countdown_.set(std::to_string(secondsToDisplay(session.countdown())));
        countdown_.drawCentered(screenCenterX, (config::windowHeight - countdown_.height()) / 2);
    } else {
        const int seconds = secondsToDisplay(session.timeLeft());
        timer_.set(std::to_string(seconds));
        if (seconds < config::lowTimeThreshold) {
            timer_.setColor(config::red);
        } else {
            timer_.setColor(config::white);
        }
        timer_.drawRightAligned(config::windowWidth - hudMargin, hudMargin);
        drawDashGauge();
    }
}

// Barre en bas à gauche : se remplit pendant la recharge, passe en vert quand le dash est prêt.
void PlayingScreen::drawDashGauge() const {
    SDL_Renderer* renderer = context_.renderer;
    const float readiness = context_.session.dashReadiness();

    const int barY = config::windowHeight - hudMargin - dashGaugeHeight;
    dashLabel_.draw(hudMargin, barY - dashLabel_.height() - 4);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const SDL_Rect background{hudMargin, barY, dashGaugeWidth, dashGaugeHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, config::dimOpacity);
    SDL_RenderFillRect(renderer, &background);

    SDL_Color fill = config::white;
    if (readiness >= 1.0f) {
        fill = config::mint;
    }
    const SDL_Rect filled{hudMargin, barY,
                          static_cast<int>(static_cast<float>(dashGaugeWidth) * readiness),
                          dashGaugeHeight};
    SDL_SetRenderDrawColor(renderer, fill.r, fill.g, fill.b, fill.a);
    SDL_RenderFillRect(renderer, &filled);
}
