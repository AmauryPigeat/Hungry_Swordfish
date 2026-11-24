#pragma once

#include "assets.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "text.hpp"

#include <cstdint>
#include <random>
#include <vector>

enum class BonusType : std::uint8_t {
    Sprint,       // nage plus vite pendant quelques secondes
    DoubleScore,  // points multipliés pendant quelques secondes
};

inline constexpr int bonusTypeCount = 2;

struct Bonus {
    Vec2 position;  // centre de la bulle, en coordonnées du monde
    BonusType type;
    float phase;  // décalage du flottement : les bulles ne montent pas toutes en même temps
};

// Les bulles bonus réparties dans le monde. Comme les poissons, elles n'apparaissent
// jamais sous les yeux du joueur.
class BonusField {
public:
    BonusField(SDL_Renderer* renderer, const Assets& assets);

    // Vide le monde puis le remplit, sans faire apparaître de bulle dans hiddenArea.
    void reset(const SDL_FRect& hiddenArea);

    // Fait flotter les bulles.
    void update(float dt);

    // Retire les bulles qui touchent hitbox et renvoie leurs types.
    std::vector<BonusType> takeTouching(const SDL_FRect& hitbox);

    // Complète jusqu'à config::bonusCount bulles, hors de hiddenArea.
    void respawnMissing(const SDL_FRect& hiddenArea);

    void draw(SDL_Renderer* renderer, const Camera& camera) const;

private:
    static constexpr float bobHeight = 6.0f;  // amplitude du flottement, en pixels
    static constexpr float bobSpeed = 2.0f;   // en radians par seconde

    Bonus randomBonus(const SDL_FRect& hiddenArea);

    const Assets& assets_;
    std::vector<Bonus> bonuses_;
    std::vector<Text> labels_;  // une étiquette par type, dans l'ordre de BonusType
    float time_ = 0.0f;
    std::mt19937 random_{std::random_device{}()};
};
