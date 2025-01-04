#include "constants.hpp"
#include "player.hpp"

#ifndef PROJET_POISSON_ENDSCREEN_HPP
#define PROJET_POISSON_ENDSCREEN_HPP


class endScreen {
private:

    SDL_Texture* logoImageTexture;
    SDL_Texture* logoTextTexture;

    SDL_Texture* scoreTextTexture;
    SDL_Texture* killCountTextTexture;

    SDL_Texture* buttonBackgroundTexture;
    SDL_Texture* quitButtonTextTexture;
    SDL_Texture* reStartButtonTextTexture;
    SDL_Texture* homeButtonTextTexture;

    SDL_Rect logoImagePos;
    SDL_Rect logoTextPos;

    SDL_Rect scoreTextPos;
    SDL_Rect killCountTextPos;
    SDL_Rect buttonHomeTextPos;
    SDL_Rect buttonQuitTextPos;
    SDL_Rect buttonReStartTextPos;


public:
    SDL_Rect buttonQuitBackPos;
    SDL_Rect buttonReStartBackPos;
    SDL_Rect buttonHomeBackPos;

    endScreen(SDL_Renderer* renderer);
    ~endScreen();

    void draw(SDL_Renderer* renderer, Player* player);
    void createScoreFile();
    void enterScore(Player* player);
};


#endif //PROJET_POISSON_ENDSCREEN_HPP
