#include "constants.hpp"



namespace FONT {
    TTF_Font *font1 = nullptr;
    TTF_Font *font2 = nullptr;
    TTF_Font *font3 = nullptr;
    SDL_Color color = {255, 255, 255, 255};
    SDL_Color color2 = {168, 255, 209, 255};
    SDL_Color color3 = {200, 50, 50, 255};
}

namespace {
    void checkFonts() {
        FONT::font1 = TTF_OpenFont("../fonts/Retro_Gaming.ttf", 32);
        if (!FONT::font1) {
            std::cout << "Erreur lors du chargement de font1: " << TTF_GetError() << std::endl;
        }
        FONT::font2 = TTF_OpenFont("../fonts/Retro_Gaming.ttf", 18);
        if (!FONT::font2) {
            std::cout << "Erreur lors du chargement de font2: " << TTF_GetError() << std::endl;
        }
        FONT::font3 = TTF_OpenFont("../fonts/Retro_Gaming.ttf", 60);
        if (!FONT::font3) {
            std::cout << "Erreur lors du chargement de font3: " << TTF_GetError() << std::endl;
        }
    }
}

// Appelez cette fonction dans `main` ou après TTF_Init()
void initFonts() {
    checkFonts();
}
