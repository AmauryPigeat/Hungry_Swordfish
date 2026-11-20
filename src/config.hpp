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
inline constexpr float playerHitboxSize = 50.0f;  // corps : ramasse les bulles bonus

// Mâchoire : seule cette zone, à l'avant de la tête, mange les poissons.
// Décalage depuis le centre du sprite, image tournée vers la droite, à la taille de départ
// (le sprite fait 88 x 44 : la tête finit vers x = +24, l'épée va jusqu'à x = +44).
inline constexpr float jawOffsetX = 30.0f;  // vers l'avant
inline constexpr float jawOffsetY = 2.0f;   // vers le ventre
inline constexpr float jawHitboxWidth = 28.0f;   // le long du poisson
inline constexpr float jawHitboxHeight = 14.0f;  // en travers du poisson
inline constexpr bool showJawHitbox = true;  // debug : dessine la mâchoire en bleu

// Dash (touche Espace) : courte poussée, puis temps de recharge
inline constexpr float dashSpeedMultiplier = 2.5f;
inline constexpr float dashDuration = 0.25f;
inline constexpr float dashCooldown = 2.0f;  // compté depuis le début du dash

// Croissance : l'espadon grossit à chaque poisson mangé (sprite et hitbox)
inline constexpr float playerGrowthPerKill = 0.008f;  // +0,8 % de taille par poisson
inline constexpr float playerMaxScale = 1.8f;

// Poissons
inline constexpr std::size_t fishCount = 1000;

// Bonus ramassables
inline constexpr std::size_t bonusCount = 6;  // présents en même temps dans le monde
inline constexpr float bonusSize = 48.0f;
inline constexpr float bonusEffectDuration = 5.0f;  // sprint et score x2
inline constexpr float bonusExtraTime = 5.0f;       // secondes ajoutées au chrono
inline constexpr float sprintMultiplier = 1.5f;
inline constexpr int scoreMultiplier = 2;

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
inline constexpr SDL_Color timeBonusColor{230, 120, 0, 255};
inline constexpr SDL_Color sprintBonusColor{20, 60, 160, 255};
inline constexpr SDL_Color scoreBonusColor{170, 0, 120, 255};
inline constexpr SDL_Color gold{255, 205, 60, 255};  // podium de l'écran des scores
inline constexpr SDL_Color silver{205, 215, 225, 255};
inline constexpr SDL_Color bronze{215, 140, 75, 255};
inline constexpr Uint8 dimOpacity = 200;  // assombrissement (pause, décompte)

}  // namespace config
