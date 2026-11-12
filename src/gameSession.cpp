#include "gameSession.hpp"

#include "config.hpp"

#include <algorithm>

GameSession::GameSession(const Assets& assets) : assets_(assets), player_(assets), fish_(assets) {}

void GameSession::start(const std::string& playerName) {
    playerName_ = playerName;
    countdown_ = config::countdownDuration;
    timeLeft_ = config::roundDuration;
    score_ = 0;
    kills_ = 0;

    player_.reset();
    camera_.centerOn(player_.position());
    fish_.reset(noSpawnArea());
}

void GameSession::update(float dt, Vec2 direction) {
    if (isOver()) {
        return;
    }

    // Les poissons nagent déjà pendant le décompte, mais le joueur attend.
    fish_.update(player_.position());
    if (inCountdown()) {
        countdown_ -= dt;
        return;
    }

    player_.update(dt, direction);
    camera_.centerOn(player_.position());

    const EatResult eaten = fish_.eatTouching(player_.hitbox());
    kills_ += eaten.count;
    score_ += eaten.points;
    fish_.respawnMissing(noSpawnArea());

    timeLeft_ = std::max(0.0f, timeLeft_ - dt);
}

void GameSession::draw(SDL_Renderer* renderer) const {
    // Le fond couvre tout le monde : on en affiche la partie vue par la caméra.
    int textureWidth = 0;
    int textureHeight = 0;
    SDL_QueryTexture(assets_.background.get(), nullptr, nullptr, &textureWidth, &textureHeight);
    const float scaleX = static_cast<float>(textureWidth) / config::worldWidth;
    const float scaleY = static_cast<float>(textureHeight) / config::worldHeight;
    const SDL_Rect source{
        static_cast<int>(camera_.x * scaleX),
        static_cast<int>(camera_.y * scaleY),
        static_cast<int>(config::windowWidth * scaleX),
        static_cast<int>(config::windowHeight * scaleY),
    };
    SDL_RenderCopy(renderer, assets_.background.get(), &source, nullptr);

    fish_.draw(renderer, camera_);
    player_.draw(renderer, camera_);
}

SDL_FRect GameSession::noSpawnArea() const {
    constexpr float margin = 60.0f;
    const SDL_FRect view = camera_.view();
    return {view.x - margin, view.y - margin, view.w + 2 * margin, view.h + 2 * margin};
}
