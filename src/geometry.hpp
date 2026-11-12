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
