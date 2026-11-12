#pragma once

#include "assets.hpp"
#include "camera.hpp"
#include "geometry.hpp"

#include <array>
#include <cstdint>
#include <random>
#include <vector>

// Caractéristiques d'une espèce de poisson.
struct Species {
    float width;
    float height;
    int points;  // score gagné quand le joueur le mange
};

inline constexpr std::array<Species, Assets::fishSpeciesCount> speciesTable{{
    {30.0f, 24.0f, 1},
    {38.0f, 30.0f, 2},
    {45.0f, 36.0f, 3},
}};

struct Boid {
    float x, y;    // centre du poisson, en coordonnées du monde
    float vx, vy;  // vitesse en pixels par tick
    std::uint8_t species;
    std::uint8_t animationTick;
};

struct EatResult {
    int count = 0;
    int points = 0;
};

// Banc de poissons simulé avec l'algorithme des boids :
// chaque poisson se rapproche de ses voisins, s'aligne sur eux et évite les collisions.
class BoidSimulation {
public:
    explicit BoidSimulation(const Assets& assets);

    // Vide le banc puis le remplit, sans faire apparaître de poisson dans hiddenArea.
    void reset(const SDL_FRect& hiddenArea);

    // Avance la simulation d'un tick (voir config::tickRate).
    // predator : position du joueur, que les poissons proches cherchent à fuir.
    void update(Vec2 predator);

    // Retire les poissons qui touchent hitbox et renvoie ce qu'ils rapportent.
    EatResult eatTouching(const SDL_FRect& hitbox);

    // Complète le banc jusqu'à config::fishCount, hors de hiddenArea
    // pour que les poissons n'apparaissent pas sous les yeux du joueur.
    void respawnMissing(const SDL_FRect& hiddenArea);

    void draw(SDL_Renderer* renderer, const Camera& camera) const;

    std::size_t size() const { return boids_.size(); }

private:
    // Réglages du comportement, par tick.
    static constexpr float visualRange = 100.0f;    // distance à laquelle un poisson voit les autres
    static constexpr float protectedRange = 40.0f;  // distance en dessous de laquelle il s'écarte
    static constexpr float centeringFactor = 0.001f;
    static constexpr float matchingFactor = 0.05f;
    static constexpr float avoidFactor = 0.15f;
    static constexpr float turnFactor = 0.2f;
    static constexpr float edgeMargin = 100.0f;  // il fait demi-tour avant de toucher le bord
    static constexpr float fleeRange = 160.0f;   // distance à laquelle il repère le joueur
    static constexpr float fleeFactor = 0.5f;    // force de la fuite (0 = les poissons ignorent le joueur)
    static constexpr float minSpeed = 2.0f;
    static constexpr float maxSpeed = 6.0f;
    static constexpr int ticksPerFrame = 4;  // vitesse de l'animation

    // Grille qui range les poissons par case de la taille de visualRange.
    // Un poisson ne regarde que les 9 cases autour de lui au lieu de tout le banc.
    static constexpr float cellSize = visualRange;
    static constexpr int gridColumns = static_cast<int>(config::worldWidth / cellSize) + 1;
    static constexpr int gridRows = static_cast<int>(config::worldHeight / cellSize) + 1;

    static int columnOf(float x);
    static int rowOf(float y);
    void rebuildGrid();
    void steer(Boid& boid, Vec2 predator) const;
    Boid randomBoid(const SDL_FRect& hiddenArea);

    const Assets& assets_;
    std::vector<Boid> boids_;
    std::mt19937 random_{std::random_device{}()};

    // Les poissons de la case c sont boidsByCell_[cellStart_[c]] ... boidsByCell_[cellStart_[c + 1] - 1].
    std::vector<int> cellStart_;
    std::vector<int> cellCursor_;
    std::vector<int> boidsByCell_;
};
