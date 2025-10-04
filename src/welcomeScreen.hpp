#include "constants.hpp"

#ifndef PROJET_POISSON_WELCOMESCREEN_HPP
#define PROJET_POISSON_WELCOMESCREEN_HPP


class WelcomeScreen {
private:
    SDL_Texture* logoImageTexture;
    SDL_Texture* logoTextTexture;

    SDL_Texture* buttonBackgroundTexture;

    SDL_Texture* startButtonTextTexture;
    SDL_Texture* scoreButtonTextTexture;
    SDL_Texture* quitButtonTextTexture;

    SDL_Rect logoImagePos;
    SDL_Rect logoTextPos;
public:

    SDL_Rect buttonStartTextPos;
    SDL_Rect buttonStartBackPos;

    SDL_Rect buttonScoreTextPos;
    SDL_Rect buttonScoreBackPos;

    SDL_Rect buttonQuitTextPos;
    SDL_Rect buttonQuitBackPos;

    WelcomeScreen(SDL_Renderer* renderer);
    ~WelcomeScreen();

    void drawWelcomeScreen(SDL_Renderer* renderer);
};


#endif //PROJET_POISSON_WELCOMESCREEN_HPP
