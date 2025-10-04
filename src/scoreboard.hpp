#include "constants.hpp"
#include "player.hpp"

#ifndef PROJET_POISSON_SCOREBOARD_HPP
#define PROJET_POISSON_SCOREBOARD_HPP


class Scoreboard {
private:
    SDL_Texture* scoreTexture;
    SDL_Texture* killCountTexture;
    SDL_Rect scorePos;
    SDL_Rect killCountPos;

public:
    Scoreboard(SDL_Renderer* renderer);
    ~Scoreboard();

    void drawPlayerScore(SDL_Renderer* renderer, Player* player);
    void drawPlayerKillCount(SDL_Renderer* renderer, Player* player);
};


#endif //PROJET_POISSON_SCOREBOARD_HPP
