#include "gameSession.hpp"

#include "config.hpp"

#include <algorithm>

GameSession::GameSession(SDL_Renderer* renderer, const Assets& assets)
    : assets_(assets), player_(assets), fish_(assets), bonuses_(renderer, assets) {}

void GameSession::start(const std::string& playerName) {
    playerName_ = playerName;
    countdown_ = config::countdownDuration;
    timeLeft_ = config::roundDuration;
    sprintTimeLeft_ = 0.0f;
    doubleScoreTimeLeft_ = 0.0f;
    score_ = 0;
    kills_ = 0;

    player_.reset();
    camera_.centerOn(player_.position());
    fish_.reset(noSpawnArea());
    bonuses_.reset(noSpawnArea());
}

void GameSession::update(float dt, const PlayerInput& input) {
    if (isOver()) {
        return;
    }

    // Les poissons nagent et les bulles flottent déjà pendant le décompte, mais le joueur attend.
    fish_.update(player_.position());
    bonuses_.update(dt);
    if (inCountdown()) {
        countdown_ -= dt;
        return;
    }

    const float speedMultiplier = sprintTimeLeft_ > 0.0f ? config::sprintMultiplier : 1.0f;
    player_.update(dt, input, speedMultiplier);
    camera_.centerOn(player_.position());

    const EatResult eaten = fish_.eatTouching(player_.jawHitbox());
    const int pointsMultiplier = doubleScoreTimeLeft_ > 0.0f ? config::scoreMultiplier : 1;
    kills_ += eaten.count;
    score_ += eaten.points * pointsMultiplier;
    player_.grow(eaten.count);

    for (const BonusType type : bonuses_.takeTouching(player_.hitbox())) {
        applyBonus(type);
    }

    fish_.respawnMissing(noSpawnArea());
    bonuses_.respawnMissing(noSpawnArea());

    timeLeft_ = std::max(0.0f, timeLeft_ - dt);
    sprintTimeLeft_ = std::max(0.0f, sprintTimeLeft_ - dt);
    doubleScoreTimeLeft_ = std::max(0.0f, doubleScoreTimeLeft_ - dt);
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

    bonuses_.draw(renderer, camera_);
    fish_.draw(renderer, camera_);
    player_.draw(renderer, camera_);
}

SDL_FRect GameSession::noSpawnArea() const {
    constexpr float margin = 60.0f;
    const SDL_FRect view = camera_.view();
    return {view.x - margin, view.y - margin, view.w + 2 * margin, view.h + 2 * margin};
}

// Les durées se cumulent : reprendre un bonus encore actif ajoute ses secondes au temps restant.
void GameSession::applyBonus(BonusType type) {
    switch (type) {
        case BonusType::ExtraTime:
            timeLeft_ += config::bonusExtraTime;
            break;
        case BonusType::Sprint:
            sprintTimeLeft_ += config::bonusEffectDuration;
            break;
        case BonusType::DoubleScore:
            doubleScoreTimeLeft_ += config::bonusEffectDuration;
            break;
    }
}
