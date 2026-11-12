#pragma once

#include "config.hpp"
#include "geometry.hpp"

#include <algorithm>
#include <cmath>

// La caméra est la partie du monde visible à l'écran.
// Pour dessiner un objet : position écran = position monde - position caméra.
struct Camera {
    float x = 0.0f;  // coin haut-gauche, en coordonnées du monde
    float y = 0.0f;

    // Centre la vue sur un point, sans jamais sortir du monde.
    void centerOn(Vec2 target) {
        const float maxX = config::worldWidth - config::windowWidth;
        const float maxY = config::worldHeight - config::windowHeight;
        x = std::clamp(std::round(target.x - config::windowWidth / 2.0f), 0.0f, maxX);
        y = std::clamp(std::round(target.y - config::windowHeight / 2.0f), 0.0f, maxY);
    }

    SDL_FRect view() const {
        return {x, y, static_cast<float>(config::windowWidth),
                static_cast<float>(config::windowHeight)};
    }
};
