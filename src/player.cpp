#include "player.hpp"

#include "config.hpp"

#include <algorithm>
#include <cmath>

namespace {

// Garde une coordonnée entre margin et worldSize - margin.
// Si l'espadon est devenu plus grand que le monde sur cet axe, il n'y a plus de place :
// on le garde au milieu (std::clamp ne marche pas quand le minimum dépasse le maximum).
float keepInside(float value, float margin, float worldSize) {
    if (2.0f * margin >= worldSize) {
        return worldSize / 2.0f;
    }
    return std::clamp(value, margin, worldSize - margin);
}

}  // namespace

Player::Player(const Assets& assets) : assets_(assets) {
    reset({config::worldWidth / 2.0f, config::worldHeight / 2.0f});
}

void Player::reset(Vec2 startPosition) {
    position_ = startPosition;
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

    // Le joueur reste entièrement à l'intérieur de la bordure rouge.
    const float margin = config::worldBorderThickness + config::playerWidth * scale_ / 2.0f;
    position_.x = keepInside(position_.x, margin, config::worldWidth);
    position_.y = keepInside(position_.y, margin, config::worldHeight);

    // À l'arrêt, on garde la dernière orientation.
    if (swimming_ || dashTimeLeft_ > 0.0f) {
        angle_ = angleInDegrees(velocity_);
    }
    animationTime_ += dt;
}

void Player::draw(SDL_Renderer* renderer, const Camera& camera) const {
    // Pendant un dash, le poisson nage même si aucune touche de direction n'est appuyée.
    const bool moving = swimming_ || dashTimeLeft_ > 0.0f;
    const Animation* animation = &assets_.playerIdle;
    if (moving) {
        animation = &assets_.playerSwim;
    }
    const auto frame = static_cast<std::size_t>(animationTime_ * config::playerAnimationFps) %
                       animation->size();

    const SDL_FRect inWorld = centeredRect(position_, config::playerWidth * scale_,
                                           config::playerHeight * scale_);
    const SDL_FRect destination = camera.toScreen(inWorld);

    // L'image regarde vers la droite. Quand le poisson va vers la gauche,
    // on la retourne pour qu'il ne nage pas sur le dos.
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (std::abs(angle_) > 90.0f) {
        flip = SDL_FLIP_VERTICAL;
    }
    SDL_RenderCopyExF(renderer, (*animation)[frame].get(), nullptr, &destination, angle_, nullptr,
                      flip);

    if (config::showJawHitbox) {
        drawJawHitbox(renderer, camera);
    }
}

// Debug : la mâchoire en bleu, remplie en transparence avec un contour.
void Player::drawJawHitbox(SDL_Renderer* renderer, const Camera& camera) const {
    const Quad jaw = jawHitbox();
    const SDL_Color blue{0, 120, 255, 255};
    const SDL_Color transparentBlue{0, 120, 255, 90};

    // Les 4 coins à l'écran. SDL_RenderGeometry dessine des triangles :
    // le rectangle est coupé en deux triangles (coins 0-1-2 et 0-2-3).
    SDL_Vertex vertices[4];
    SDL_FPoint outline[5];
    for (int i = 0; i < 4; ++i) {
        const Vec2 onScreen = camera.toScreen(jaw.corners[i]);
        const SDL_FPoint point{onScreen.x, onScreen.y};
        vertices[i].position = point;
        vertices[i].color = transparentBlue;
        vertices[i].tex_coord = {0.0f, 0.0f};
        outline[i] = point;
    }
    outline[4] = outline[0];  // on revient au premier coin pour fermer le contour

    const int triangles[6] = {0, 1, 2, 0, 2, 3};
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderGeometry(renderer, nullptr, vertices, 4, triangles, 6);

    SDL_SetRenderDrawColor(renderer, blue.r, blue.g, blue.b, blue.a);
    SDL_RenderDrawLinesF(renderer, outline, 5);
}

void Player::grow(int kills) {
    scale_ += static_cast<float>(kills) * config::playerGrowthPerKill;
}

SDL_FRect Player::hitbox() const {
    // Carré centré sur le poisson : valable quelle que soit son orientation.
    const float size = config::playerHitboxSize * scale_;
    return centeredRect(position_, size, size);
}

Quad Player::jawHitbox() const {
    // Position de la mâchoire dans l'image (tournée vers la droite), à la taille actuelle.
    Vec2 offset{config::jawOffsetX * scale_, config::jawOffsetY * scale_};

    // L'image est d'abord retournée verticalement quand le poisson va vers la gauche
    // (voir draw) : le ventre passe alors de l'autre côté.
    if (std::abs(angle_) > 90.0f) {
        offset.y = -offset.y;
    }

    // Puis elle est tournée de angle_ autour de son centre : on applique la même rotation
    // au décalage, et la mâchoire tourne du même angle que le poisson.
    const Vec2 center = position_ + rotate(offset, angle_);
    const float width = config::jawHitboxWidth * scale_;
    const float height = config::jawHitboxHeight * scale_;
    return rotatedRect(center, width, height, angle_);
}

float Player::dashReadiness() const {
    return 1.0f - dashCooldownLeft_ / config::dashCooldown;
}
