#include "bonuses.hpp"

#include "config.hpp"

#include <cmath>

BonusField::BonusField(SDL_Renderer* renderer, const Assets& assets) : assets_(assets) {
    TTF_Font* font = assets.smallFont.get();
    labels_.emplace_back(renderer, font, config::sprintBonusColor, ">>");
    labels_.emplace_back(renderer, font, config::scoreBonusColor, "x2");
    bonuses_.reserve(config::bonusCount);
}

void BonusField::reset(const SDL_FRect& hiddenArea) {
    bonuses_.clear();
    time_ = 0.0f;
    respawnMissing(hiddenArea);
}

void BonusField::update(float dt) {
    time_ += dt;
}

std::vector<BonusType> BonusField::takeTouching(const SDL_FRect& hitbox) {
    std::vector<BonusType> taken;
    std::erase_if(bonuses_, [&](const Bonus& bonus) {
        if (!overlaps(hitbox, centeredRect(bonus.position, config::bonusSize, config::bonusSize))) {
            return false;
        }
        taken.push_back(bonus.type);
        return true;
    });
    return taken;
}

void BonusField::respawnMissing(const SDL_FRect& hiddenArea) {
    while (bonuses_.size() < config::bonusCount) {
        bonuses_.push_back(randomBonus(hiddenArea));
    }
}

void BonusField::draw(SDL_Renderer* renderer, const Camera& camera) const {
    const SDL_FRect screen{0.0f, 0.0f, static_cast<float>(config::windowWidth),
                           static_cast<float>(config::windowHeight)};

    for (const Bonus& bonus : bonuses_) {
        const float bob = std::sin(time_ * bobSpeed + bonus.phase) * bobHeight;
        const Vec2 center{bonus.position.x, bonus.position.y + bob};
        const SDL_FRect inWorld = centeredRect(center, config::bonusSize, config::bonusSize);
        const SDL_FRect destination = camera.toScreen(inWorld);
        if (!overlaps(destination, screen)) {
            continue;
        }

        SDL_RenderCopyF(renderer, assets_.bonusBubble.get(), nullptr, &destination);

        // L'étiquette suit le zoom, comme la bulle, pour rester à l'intérieur.
        const Text& label = labels_[static_cast<std::size_t>(bonus.type)];
        const Vec2 onScreen = camera.toScreen(center);
        label.drawCenteredScaled(onScreen.x, onScreen.y, camera.zoom);
    }
}

Bonus BonusField::randomBonus(const SDL_FRect& hiddenArea) {
    const float margin = config::spawnEdgeMargin;
    std::uniform_real_distribution<float> randomX(margin, config::worldWidth - margin);
    std::uniform_real_distribution<float> randomY(margin, config::worldHeight - margin);
    std::uniform_real_distribution<float> randomPhase(0.0f, 6.2831853f);
    std::uniform_int_distribution<int> randomType(0, bonusTypeCount - 1);

    // Même principe que pour les poissons (voir BoidSimulation::randomBoid).
    Bonus bonus{};
    do {
        bonus.position = {randomX(random_), randomY(random_)};
    } while (contains(hiddenArea, bonus.position));

    bonus.type = static_cast<BonusType>(randomType(random_));
    bonus.phase = randomPhase(random_);
    return bonus;
}
