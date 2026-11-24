#pragma once

#include "config.hpp"
#include "geometry.hpp"

#include <algorithm>
#include <cmath>

// La caméra est la partie du monde visible à l'écran.
// Pour dessiner un objet : position écran = (position monde - position caméra) x zoom.
struct Camera {
    float x = 0.0f;  // coin haut-gauche de la vue, en coordonnées du monde
    float y = 0.0f;

    // 1 = taille normale, 0.5 = on voit deux fois plus loin (dézoom), 2 = deux fois plus près.
    float zoom = 1.0f;

    // Taille de la partie du monde visible : plus on dézoome, plus elle est grande.
    float viewWidth() const { return config::windowWidth / zoom; }
    float viewHeight() const { return config::windowHeight / zoom; }

    // Centre la vue sur un point, sans sortir du monde.
    // Si la vue est plus grande que le monde (gros dézoom en mode dev), le monde est centré.
    void centerOn(Vec2 target) {
        x = centeredAxis(target.x, viewWidth(), config::worldWidth);
        y = centeredAxis(target.y, viewHeight(), config::worldHeight);

        // Arrondi au pixel d'écran : évite que le fond "tremble" quand la caméra bouge.
        x = std::round(x * zoom) / zoom;
        y = std::round(y * zoom) / zoom;
    }

    SDL_FRect view() const { return {x, y, viewWidth(), viewHeight()}; }

    // Passe d'une position dans le monde à une position à l'écran.
    Vec2 toScreen(Vec2 world) const { return {(world.x - x) * zoom, (world.y - y) * zoom}; }

    // Pareil pour un rectangle : il est déplacé et agrandi / rétréci selon le zoom.
    SDL_FRect toScreen(const SDL_FRect& world) const {
        const Vec2 topLeft = toScreen(Vec2{world.x, world.y});
        return {topLeft.x, topLeft.y, world.w * zoom, world.h * zoom};
    }

private:
    // Position du bord gauche (ou haut) de la vue sur un axe.
    static float centeredAxis(float target, float viewSize, float worldSize) {
        if (viewSize >= worldSize) {
            return (worldSize - viewSize) / 2.0f;
        }
        return std::clamp(target - viewSize / 2.0f, 0.0f, worldSize - viewSize);
    }
};
