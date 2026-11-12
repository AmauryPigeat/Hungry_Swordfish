#pragma once

#include <SDL2/SDL.h>

#include <cstddef>

// Tous les réglages du jeu au même endroit.
// Les distances sont en pixels, les durées en secondes.
namespace config {

// Fenêtre : résolution "logique". La fenêtre peut être redimensionnée,
// SDL se charge de la mise à l'échelle.
inline constexpr int windowWidth = 1280;
inline constexpr int windowHeight = 720;
inline constexpr const char* windowTitle = "Hungry Swordfish";

// Monde dans lequel nagent le joueur et les poissons.
inline constexpr float worldWidth = 3840.0f;
inline constexpr float worldHeight = 2160.0f;

// La logique du jeu avance à pas fixe, indépendamment des FPS d'affichage.
inline constexpr float tickRate = 60.0f;
inline constexpr float tickDuration = 1.0f / tickRate;

// Déroulement d'une partie
inline constexpr float countdownDuration = 3.0f;
inline constexpr float roundDuration = 60.0f;
inline constexpr int lowTimeThreshold = 10;  // le chrono passe en rouge en dessous

// Joueur
inline constexpr float playerWidth = 88.0f;
inline constexpr float playerHeight = 44.0f;
inline constexpr float playerSpeed = 450.0f;        // pixels par seconde
inline constexpr float playerAcceleration = 10.0f;  // plus c'est grand, plus il réagit vite
inline constexpr float playerAnimationFps = 10.0f;
inline constexpr float playerHitboxSize = 50.0f;

// Poissons
inline constexpr std::size_t fishCount = 1000;

// Scores
inline constexpr std::size_t maxNameLength = 16;
inline constexpr std::size_t displayedScores = 10;
inline constexpr const char* scoreFileName = "scoreDB.txt";

// Interface
inline constexpr int buttonWidth = 221;
inline constexpr int buttonHeight = 64;
inline constexpr SDL_Color white{255, 255, 255, 255};
inline constexpr SDL_Color mint{168, 255, 209, 255};
inline constexpr SDL_Color red{200, 50, 50, 255};
inline constexpr Uint8 dimOpacity = 200;  // assombrissement (pause, décompte)

}  // namespace config
