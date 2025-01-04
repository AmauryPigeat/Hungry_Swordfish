#include "constants.hpp"

#ifndef PROJET_POISSON_PAUSEDSCREEN_HPP
#define PROJET_POISSON_PAUSEDSCREEN_HPP


class pauseScreen {
private:

    SDL_Texture* logoImageTexture;
    SDL_Texture* logoTextTexture;

    SDL_Texture* pauseTextTexture;

    SDL_Texture* buttonBackgroundTexture;
    SDL_Texture* quitButtonTextTexture;
    SDL_Texture* resumeButtonTextTexture;

    SDL_Rect logoImagePos;
    SDL_Rect logoTextPos;
    SDL_Rect pauseTextPos;
public:
    SDL_Rect buttonQuitTextPos;
    SDL_Rect buttonQuitBackPos;
    SDL_Rect buttonResumeTextPos;
    SDL_Rect buttonResumeBackPos;

    pauseScreen(SDL_Renderer* renderer);
    ~pauseScreen();

    void draw(SDL_Renderer* renderer);


};




#endif //PROJET_POISSON_PAUSEDSCREEN_HPP
