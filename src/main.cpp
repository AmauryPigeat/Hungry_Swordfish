#include "game.hpp"

#include <exception>
#include <iostream>

int main(int /*argc*/, char* /*argv*/[]) {
    try {
        Game game;
        game.run();
    } catch (const std::exception& error) {
        // Toute erreur de démarrage (SDL, image ou police manquante...) arrive ici.
        std::cerr << "Erreur : " << error.what() << '\n';
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Hungry Swordfish", error.what(), nullptr);
        return 1;
    }
    return 0;
}
