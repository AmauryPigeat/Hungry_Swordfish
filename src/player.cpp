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
    facing_ = {1.0f, 0.0f};
    angle_ = 0.0f;
    swimming_ = false;
    animationTime_ = 0.0f;
    scale_ = 1.0f;
    dashDirection_ = {};
    dashTimeLeft_ = 0.0f;
    dashCooldownLeft_ = 0.0f;
}

void Player::update(float dt, const PlayerInput& input, float speedMultiplier) {
    const float speed = config::playerSpeed * speedMultiplier;

    // Vitesse visée : même norme dans toutes les directions, diagonales comprises.
    Vec2 targetVelocity;
    const float directionLength = length(input.direction);
    swimming_ = directionLength > 0.0f;
    if (swimming_) {
        facing_ = input.direction * (1.0f / directionLength);
        targetVelocity = facing_ * speed;
    }

    // Dash : dans le sens demandé, ou droit devant si le joueur est à l'arrêt.
    if (input.dash && dashCooldownLeft_ <= 0.0f) {
        dashDirection_ = facing_;
        dashTimeLeft_ = config::dashDuration;
        dashCooldownLeft_ = config::dashCooldown;
    }

    if (dashTimeLeft_ > 0.0f) {
        // Pendant le dash, la vitesse est imposée. À la fin, le lissage ci-dessous
        // la ramène en douceur à la vitesse normale.
        velocity_ = dashDirection_ * (speed * config::dashSpeedMultiplier);
        dashTimeLeft_ -= dt;
    } else {
        // On se rapproche progressivement de la vitesse visée : départs et arrêts en douceur.
        const float blend = std::min(1.0f, config::playerAcceleration * dt);
        velocity_ = velocity_ + (targetVelocity - velocity_) * blend;
    }
    dashCooldownLeft_ = std::max(0.0f, dashCooldownLeft_ - dt);
    position_ = position_ + velocity_ * dt;

    // Le joueur reste entièrement dans le monde.
    const float margin = config::playerWidth * scale_ / 2.0f;
    position_.x = std::clamp(position_.x, margin, config::worldWidth - margin);
    position_.y = std::clamp(position_.y, margin, config::worldHeight - margin);

    // À l'arrêt, on garde la dernière orientation.
    if (swimming_ || dashTimeLeft_ > 0.0f) {
        angle_ = angleInDegrees(velocity_);
    }
    animationTime_ += dt;
}

void Player::draw(SDL_Renderer* renderer, const Camera& camera) const {
    // Pendant un dash, le poisson nage même si aucune touche de direction n'est appuyée.
    const bool moving = swimming_ || dashTimeLeft_ > 0.0f;
    const Animation& animation = moving ? assets_.playerSwim : assets_.playerIdle;
    const auto frame = static_cast<std::size_t>(animationTime_ * config::playerAnimationFps) %
                       animation.size();

    const Vec2 onScreen = position_ - Vec2{camera.x, camera.y};
    const SDL_FRect destination = centeredRect(onScreen, config::playerWidth * scale_,
                                               config::playerHeight * scale_);

    // L'image regarde vers la droite. Quand le poisson va vers la gauche,
    // on la retourne pour qu'il ne nage pas sur le dos.
    const bool facingLeft = std::abs(angle_) > 90.0f;
    const SDL_RendererFlip flip = facingLeft ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
    SDL_RenderCopyExF(renderer, animation[frame].get(), nullptr, &destination, angle_, nullptr,
                      flip);
}

void Player::grow(int kills) {
    scale_ = std::min(config::playerMaxScale,
                      scale_ + static_cast<float>(kills) * config::playerGrowthPerKill);
}

SDL_FRect Player::hitbox() const {
    // Carré centré sur le poisson : valable quelle que soit son orientation.
    const float size = config::playerHitboxSize * scale_;
    return centeredRect(position_, size, size);
}

float Player::dashReadiness() const {
    return 1.0f - dashCooldownLeft_ / config::dashCooldown;
}
