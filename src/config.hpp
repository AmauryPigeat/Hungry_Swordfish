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

// Bordure rouge le long des bords du monde : ni le joueur ni les poissons ne la traversent.
inline constexpr float worldBorderThickness = 15.0f;
inline constexpr SDL_Color worldBorderColor{220, 30, 30, 255};

// La logique du jeu avance à pas fixe, indépendamment des FPS d'affichage.
inline constexpr float tickRate = 60.0f;
inline constexpr float tickDuration = 1.0f / tickRate;

// Déroulement d'une partie : pas de limite de temps, la partie dure jusqu'à HOME (menu pause).
inline constexpr float countdownDuration = 3.0f;  // décompte 3, 2, 1 avant de pouvoir nager

// Joueur
inline constexpr float playerWidth = 88.0f;
inline constexpr float playerHeight = 44.0f;
inline constexpr float playerSpeed = 450.0f;        // pixels par seconde
inline constexpr float playerAcceleration = 10.0f;  // plus c'est grand, plus il réagit vite
inline constexpr float playerAnimationFps = 10.0f;
inline constexpr float playerHitboxSize = 50.0f;  // corps : ramasse les bulles bonus
inline constexpr float playerSpawnMargin = 200.0f;  // le joueur apparaît au hasard, pas collé au bord

// Mâchoire : seule cette zone, à l'avant de la tête, mange les poissons.
// Décalage depuis le centre du sprite, image tournée vers la droite, à la taille de départ
// (le sprite fait 88 x 44 : la tête finit vers x = +24, l'épée va jusqu'à x = +44).
inline constexpr float jawOffsetX = 30.0f;  // vers l'avant
inline constexpr float jawOffsetY = 2.0f;   // vers le ventre
inline constexpr float jawHitboxWidth = 28.0f;   // le long du poisson
inline constexpr float jawHitboxHeight = 14.0f;  // en travers du poisson
inline constexpr bool showJawHitbox = false;  // debug : dessine la mâchoire en bleu

// Dash (touche Espace) : courte poussée, puis temps de recharge
inline constexpr float dashSpeedMultiplier = 2.5f;
inline constexpr float dashDuration = 0.25f;
inline constexpr float dashCooldown = 2.0f;  // compté depuis le début du dash

// Croissance : l'espadon grossit à chaque poisson mangé (sprite et hitbox), sans limite
inline constexpr float playerGrowthPerKill = 0.008f;  // +0,8 % de taille par poisson

// Caméra
// Dézoom auto : quand l'espadon grossit, la caméra recule d'une partie de sa croissance.
// 0 = pas de dézoom, 1 = l'espadon garde toujours la même taille à l'écran.
inline constexpr float autoZoomStrength = 0.5f;
inline constexpr float wheelZoomStep = 1.1f;  // un cran de molette = zoom x1,1
inline constexpr float maxWheelZoom = 2.0f;   // zoom max à la molette, par rapport au zoom auto
inline constexpr float zoomSmoothing = 8.0f;  // vitesse de la transition (plus grand = plus sec)

// Mode dev :
//   - pas de dézoom auto quand l'espadon grossit ;
//   - la molette zoome et dézoome sans limite de jeu
//     (bornée seulement entre devMinZoom et devMaxZoom pour ne pas diviser par zéro) ;
//   - poissons et bulles peuvent apparaître dans la vue du joueur.
inline constexpr bool devMode = true;
inline constexpr float devMinZoom = 0.02f;
inline constexpr float devMaxZoom = 50.0f;

// Poissons et bulles apparaissent au hasard, à au moins spawnEdgeMargin du bord du monde.
inline constexpr float spawnEdgeMargin = 100.0f;

// Poissons
inline constexpr std::size_t fishCount = 1000;

// Bonus ramassables
inline constexpr std::size_t bonusCount = 6;  // présents en même temps dans le monde
inline constexpr float bonusSize = 48.0f;
inline constexpr float bonusEffectDuration = 5.0f;  // sprint et score x2
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
inline constexpr SDL_Color sprintBonusColor{20, 60, 160, 255};
inline constexpr SDL_Color scoreBonusColor{170, 0, 120, 255};
inline constexpr SDL_Color gold{255, 205, 60, 255};  // podium de l'écran des scores
inline constexpr SDL_Color silver{205, 215, 225, 255};
inline constexpr SDL_Color bronze{215, 140, 75, 255};
inline constexpr Uint8 dimOpacity = 200;  // assombrissement (pause, décompte)

}  // namespace config
