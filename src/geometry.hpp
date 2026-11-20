#pragma once

#include <SDL2/SDL.h>

#include <cmath>

// Petit vecteur 2D, juste ce dont le jeu a besoin.
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

inline Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
inline Vec2 operator-(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
inline Vec2 operator*(Vec2 v, float k) { return {v.x * k, v.y * k}; }

inline float length(Vec2 v) { return std::sqrt(v.x * v.x + v.y * v.y); }

// Angle du vecteur en degrés, dans le sens attendu par SDL_RenderCopyEx.
inline float angleInDegrees(Vec2 v) {
    constexpr float radToDeg = 57.29577951f;
    return std::atan2(v.y, v.x) * radToDeg;
}

inline bool overlaps(const SDL_FRect& a, const SDL_FRect& b) {
    return a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h;
}

inline bool contains(const SDL_FRect& rect, Vec2 point) {
    return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y &&
           point.y < rect.y + rect.h;
}

// Rectangle de taille donnée, centré sur un point.
inline SDL_FRect centeredRect(Vec2 center, float width, float height) {
    return {center.x - width / 2.0f, center.y - height / 2.0f, width, height};
}

inline float dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }

// Tourne un vecteur de angleDegrees autour de l'origine (même sens que SDL_RenderCopyEx).
inline Vec2 rotate(Vec2 v, float angleDegrees) {
    constexpr float degToRad = 0.01745329252f;
    const float c = std::cos(angleDegrees * degToRad);
    const float s = std::sin(angleDegrees * degToRad);
    return {v.x * c - v.y * s, v.x * s + v.y * c};
}

// Rectangle qui peut être tourné, décrit par ses 4 coins dans l'ordre du tour.
// SDL_FRect ne peut pas tourner, d'où cette structure.
struct Quad {
    Vec2 corners[4];
};

// Le même rectangle qu'un SDL_FRect, sous forme de Quad.
inline Quad quadFromRect(const SDL_FRect& rect) {
    Quad quad;
    quad.corners[0] = {rect.x, rect.y};
    quad.corners[1] = {rect.x + rect.w, rect.y};
    quad.corners[2] = {rect.x + rect.w, rect.y + rect.h};
    quad.corners[3] = {rect.x, rect.y + rect.h};
    return quad;
}

// Rectangle de taille width x height, centré sur center, tourné de angleDegrees.
inline Quad rotatedRect(Vec2 center, float width, float height, float angleDegrees) {
    const float halfW = width / 2.0f;
    const float halfH = height / 2.0f;
    Quad quad;
    quad.corners[0] = center + rotate({-halfW, -halfH}, angleDegrees);
    quad.corners[1] = center + rotate({halfW, -halfH}, angleDegrees);
    quad.corners[2] = center + rotate({halfW, halfH}, angleDegrees);
    quad.corners[3] = center + rotate({-halfW, halfH}, angleDegrees);
    return quad;
}

// Projette les 4 coins sur un axe : min et max reçoivent les deux bouts de "l'ombre" du Quad.
inline void projectOnAxis(const Quad& quad, Vec2 axis, float& min, float& max) {
    min = dot(quad.corners[0], axis);
    max = min;
    for (int i = 1; i < 4; ++i) {
        const float position = dot(quad.corners[i], axis);
        if (position < min) {
            min = position;
        }
        if (position > max) {
            max = position;
        }
    }
}

// Est-ce que deux Quad se touchent ?
// Principe (théorème des axes séparateurs) : on regarde "l'ombre" des deux rectangles
// sur chacun de leurs côtés. Si sur un seul de ces axes les ombres ne se chevauchent pas,
// il y a un espace entre eux : ils ne se touchent pas. Sinon, ils se touchent.
inline bool quadsOverlap(const Quad& a, const Quad& b) {
    // Deux côtés par rectangle suffisent : les deux autres leur sont parallèles.
    const Vec2 axes[4] = {
        a.corners[1] - a.corners[0],
        a.corners[3] - a.corners[0],
        b.corners[1] - b.corners[0],
        b.corners[3] - b.corners[0],
    };

    for (int i = 0; i < 4; ++i) {
        float minA = 0.0f;
        float maxA = 0.0f;
        float minB = 0.0f;
        float maxB = 0.0f;
        projectOnAxis(a, axes[i], minA, maxA);
        projectOnAxis(b, axes[i], minB, maxB);
        if (maxA < minB || maxB < minA) {
            return false;  // espace trouvé entre les deux
        }
    }
    return true;
}
