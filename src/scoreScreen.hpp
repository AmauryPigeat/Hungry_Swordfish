#include "constants.hpp"

#ifndef PROJET_POISSON_SCORESCREEN_HPP
#define PROJET_POISSON_SCORESCREEN_HPP

struct Score{
    std::string name;
    int scorePoint;
    int killCount;
};

class ScoreScreen {
private:
    SDL_Texture* logoImageTexture;
    SDL_Texture* logoTextTexture;

    SDL_Texture* mainTextTexture;

    SDL_Texture* buttonBackgroundTexture;
    SDL_Texture* quitButtonTextTexture;
    SDL_Texture* homeButtonTextTexture;
    SDL_Texture* deleteAllButtonTextTexture;
    SDL_Texture* deleteAllButtonBackTexture;


    SDL_Rect logoImagePos;
    SDL_Rect logoTextPos;
    SDL_Rect mainTextPos;
    SDL_Rect buttonQuitTextPos;
    SDL_Rect buttonHomeTextPos;
    SDL_Rect buttonDeleteTextPos;

public:

    SDL_Rect buttonQuitBackPos;
    SDL_Rect buttonHomeBackPos;
    SDL_Rect buttonDeleteBackPos;

    ScoreScreen(SDL_Renderer* renderer);
    ~ScoreScreen();


    Score* getEveryScore();
    void draw(SDL_Renderer* renderer);
    void deleteScore();
};

// Fonction obtenue grâce à ChatGPT pour séparer une ligne et la rentrer dans une struct "score"
Score parseLineToScore(const std::string& ligne);

#endif //PROJET_POISSON_SCORESCREEN_HPP
