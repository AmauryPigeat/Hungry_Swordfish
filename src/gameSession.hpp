#pragma once

#include "assets.hpp"
#include "boidSimulation.hpp"
#include "camera.hpp"
#include "player.hpp"

#include <string>

// Une partie : le joueur, les poissons, le chrono et le score.
// Ne connaît ni les menus ni le clavier, uniquement les règles du jeu.
class GameSession {
public:
    explicit GameSession(const Assets& assets);

    // Démarre une nouvelle partie (décompte puis chrono).
    void start(const std::string& playerName);

    // Avance la partie de dt secondes. direction : sens voulu par le joueur.
    void update(float dt, Vec2 direction);

    // Dessine le monde (fond, poissons, joueur), sans l'interface.
    void draw(SDL_Renderer* renderer) const;

    bool inCountdown() const { return countdown_ > 0.0f; }
    bool isOver() const { return timeLeft_ <= 0.0f; }
    float countdown() const { return countdown_; }
    float timeLeft() const { return timeLeft_; }

    const std::string& playerName() const { return playerName_; }
    int score() const { return score_; }
    int kills() const { return kills_; }

private:
    // Zone dans laquelle aucun poisson ne doit apparaître : l'écran et un peu autour.
    SDL_FRect noSpawnArea() const;

    const Assets& assets_;
    Player player_;
    BoidSimulation fish_;
    Camera camera_;

    std::string playerName_;
    float countdown_ = 0.0f;
    float timeLeft_ = 0.0f;
    int score_ = 0;
    int kills_ = 0;
};
