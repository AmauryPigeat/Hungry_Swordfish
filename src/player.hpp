#pragma once

#include "assets.hpp"
#include "camera.hpp"
#include "geometry.hpp"

// Ce que le joueur demande pendant un tick.
struct PlayerInput {
    Vec2 direction;     // sens voulu (pas forcément normalisé), {0, 0} = à l'arrêt
    bool dash = false;  // touche de dash appuyée depuis le tick précédent
};

class Player {
public:
    explicit Player(const Assets& assets);

    // Replace le joueur au centre du monde, à l'arrêt, à sa taille de départ.
    void reset();

    // speedMultiplier : 1 en temps normal, plus avec le bonus de sprint.
    void update(float dt, const PlayerInput& input, float speedMultiplier);
    void draw(SDL_Renderer* renderer, const Camera& camera) const;

    // Fait grossir l'espadon de kills poissons mangés, sans dépasser config::playerMaxScale.
    void grow(int kills);

    Vec2 position() const { return position_; }
    SDL_FRect hitbox() const;     // tout le corps : ramasse les bonus
    SDL_FRect jawHitbox() const;  // avant de la tête : mange les poissons

    // 0 juste après un dash, 1 quand il est de nouveau disponible.
    float dashReadiness() const;

private:
    const Assets& assets_;
    Vec2 position_;  // centre du poisson, en coordonnées du monde
    Vec2 velocity_;
    Vec2 facing_;         // direction de nage la plus récente, de longueur 1
    float angle_ = 0.0f;  // orientation en degrés, 0 = vers la droite
    bool swimming_ = false;
    float animationTime_ = 0.0f;
    float scale_ = 1.0f;

    Vec2 dashDirection_;
    float dashTimeLeft_ = 0.0f;      // > 0 pendant le dash
    float dashCooldownLeft_ = 0.0f;  // > 0 tant que le dash se recharge
};
