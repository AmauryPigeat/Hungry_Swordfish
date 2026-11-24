#pragma once

#include "assets.hpp"
#include "boidSimulation.hpp"
#include "bonuses.hpp"
#include "camera.hpp"
#include "player.hpp"

#include <random>
#include <string>

// Une partie : le joueur, les poissons, les bonus et le score. Pas de limite de temps.
// Ne connaît ni les menus ni le clavier, uniquement les règles du jeu.
class GameSession {
public:
    GameSession(SDL_Renderer* renderer, const Assets& assets);

    // Démarre une nouvelle partie (décompte puis chrono).
    void start(const std::string& playerName);

    // Avance la partie de dt secondes. input : ce que demande le joueur.
    void update(float dt, const PlayerInput& input);

    // Dessine le monde (fond, bonus, poissons, joueur), sans l'interface.
    void draw(SDL_Renderer* renderer) const;

    // Molette : steps > 0 zoome, steps < 0 dézoome.
    void zoomWithWheel(int steps);

    bool inCountdown() const { return countdown_ > 0.0f; }
    float countdown() const { return countdown_; }

    // Temps restant des effets de bonus, 0 s'ils sont inactifs.
    float sprintTimeLeft() const { return sprintTimeLeft_; }
    float doubleScoreTimeLeft() const { return doubleScoreTimeLeft_; }
    float dashReadiness() const { return player_.dashReadiness(); }

    const std::string& playerName() const { return playerName_; }
    int score() const { return score_; }
    int kills() const { return kills_; }

private:
    // Zone dans laquelle aucun poisson ni bonus ne doit apparaître : l'écran et un peu autour.
    // En mode dev, zone vide : ils peuvent apparaître n'importe où.
    SDL_FRect noSpawnArea() const;
    void applyBonus(BonusType type);
    Vec2 randomPlayerStart();
    void drawWorldBorder(SDL_Renderer* renderer) const;

    // Zoom voulu par le jeu selon la taille de l'espadon (1 au départ, plus petit ensuite).
    float autoZoom() const;
    // Rapproche le zoom de la caméra du zoom voulu, puis la recentre sur le joueur.
    void updateCamera(float dt);

    const Assets& assets_;
    Player player_;
    BoidSimulation fish_;
    BonusField bonuses_;
    Camera camera_;
    float wheelZoom_ = 1.0f;  // choix du joueur à la molette, multiplié par le zoom auto
    std::mt19937 random_{std::random_device{}()};

    std::string playerName_;
    float countdown_ = 0.0f;
    float sprintTimeLeft_ = 0.0f;
    float doubleScoreTimeLeft_ = 0.0f;
    int score_ = 0;
    int kills_ = 0;
};
