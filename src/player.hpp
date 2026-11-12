#pragma once

#include "assets.hpp"
#include "camera.hpp"
#include "geometry.hpp"

class Player {
public:
    explicit Player(const Assets& assets);

    // Replace le joueur au centre du monde, à l'arrêt.
    void reset();

    // direction : sens voulu par le joueur (pas forcément normalisé), {0, 0} = à l'arrêt.
    void update(float dt, Vec2 direction);
    void draw(SDL_Renderer* renderer, const Camera& camera) const;

    Vec2 position() const { return position_; }
    SDL_FRect hitbox() const;

private:
    const Assets& assets_;
    Vec2 position_;  // centre du poisson, en coordonnées du monde
    Vec2 velocity_;
    float angle_ = 0.0f;  // orientation en degrés, 0 = vers la droite
    bool swimming_ = false;
    float animationTime_ = 0.0f;
};
