#pragma once

#include "sdlUtils.hpp"

#include <array>
#include <filesystem>
#include <vector>

// Suite d'images jouées en boucle.
using Animation = std::vector<TexturePtr>;

// Charge une seule fois toutes les images et polices du jeu.
// Les autres classes reçoivent une référence dessus et n'ont rien à libérer.
class Assets {
public:
    static constexpr std::size_t fishSpeciesCount = 3;

    explicit Assets(SDL_Renderer* renderer);

    TexturePtr background;
    TexturePtr logo;
    TexturePtr button;
    TexturePtr deleteButton;
    TexturePtr bonusBubble;

    Animation playerSwim;
    Animation playerIdle;
    std::array<Animation, fishSpeciesCount> fish;

    FontPtr smallFont;   // lignes de score, aides
    FontPtr mediumFont;  // texte courant, boutons
    FontPtr largeFont;   // décompte de début de partie

private:
    // Dossier qui contient img/ et fonts/ : à côté de l'exécutable,
    // ou un cran au-dessus (cas d'un lancement depuis cmake-build-debug/).
    static std::filesystem::path findRootDirectory();
};
