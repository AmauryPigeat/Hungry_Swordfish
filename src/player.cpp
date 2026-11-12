#include "player.hpp"

#include "config.hpp"

#include <algorithm>
#include <cmath>

Player::Player(const Assets& assets) : assets_(assets) {
    reset();
}

void Player::reset() {
    position_ = {config::worldWidth / 2.0f, config::worldHeight / 2.0f};
    velocity_ = {};
    angle_ = 0.0f;
    swimming_ = false;
    animationTime_ = 0.0f;
}

void Player::update(float dt, Vec2 direction) {
    // Vitesse visée : même norme dans toutes les directions, diagonales comprises.
    Vec2 targetVelocity;
    const float directionLength = length(direction);
    swimming_ = directionLength > 0.0f;
    if (swimming_) {
        targetVelocity = direction * (config::playerSpeed / directionLength);
    }

    // On se rapproche progressivement de la vitesse visée : départs et arrêts en douceur.
    const float blend = std::min(1.0f, config::playerAcceleration * dt);
    velocity_ = velocity_ + (targetVelocity - velocity_) * blend;
    position_ = position_ + velocity_ * dt;

    // Le joueur reste entièrement dans le monde.
    const float margin = config::playerWidth / 2.0f;
    position_.x = std::clamp(position_.x, margin, config::worldWidth - margin);
    position_.y = std::clamp(position_.y, margin, config::worldHeight - margin);

    // À l'arrêt, on garde la dernière orientation.
    if (swimming_) {
        angle_ = angleInDegrees(velocity_);
    }
    animationTime_ += dt;
}

void Player::draw(SDL_Renderer* renderer, const Camera& camera) const {
    const Animation& animation = swimming_ ? assets_.playerSwim : assets_.playerIdle;
    const auto frame = static_cast<std::size_t>(animationTime_ * config::playerAnimationFps) %
                       animation.size();

    const Vec2 onScreen = position_ - Vec2{camera.x, camera.y};
    const SDL_FRect destination =
        centeredRect(onScreen, config::playerWidth, config::playerHeight);

    // L'image regarde vers la droite. Quand le poisson va vers la gauche,
    // on la retourne pour qu'il ne nage pas sur le dos.
    const bool facingLeft = std::abs(angle_) > 90.0f;
    const SDL_RendererFlip flip = facingLeft ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
    SDL_RenderCopyExF(renderer, animation[frame].get(), nullptr, &destination, angle_, nullptr,
                      flip);
}

SDL_FRect Player::hitbox() const {
    // Carré centré sur le poisson : valable quelle que soit son orientation.
    return centeredRect(position_, config::playerHitboxSize, config::playerHitboxSize);
}
