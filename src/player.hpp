#include "constants.hpp"
#include "boidSimulation.hpp"

#ifndef PROJET_POISSON_PLAYER_HPP
#define PROJET_POISSON_PLAYER_HPP


class Player {
private:
    const int sizeW = 80;
    const int sizeH = 45;
    __int8 playerGifIter;
    unsigned int killCounter;
    unsigned int scorePoint;
    std::string name;
public:
    SDL_Rect pos;
    SDL_Rect hitbox;
    SDL_Texture* playerTextureWalk[4];
    SDL_Texture* playerTextureIdle[4];

    SDL_Texture* mainTextTexture;
    SDL_Texture* secondaryTextTexture;
    SDL_Texture* playerNameTypingTexture;

    SDL_Rect mainTextPos;
    SDL_Rect secondaryTextPos;
    SDL_Rect playerNameTypingPos;

    Player(SDL_Renderer *renderer);
    ~Player();

    void drawPlayer(SDL_Renderer *renderer, int angle, bool moving);
    void checkEveryColisions(BoidSimulation* simulation);
    void reset();
    void drawAskName(SDL_Renderer* renderer, std::string tempName);

    int getKillCount(){
        return this->killCounter;
    }

    int getScorePoint(){
        return this->scorePoint;
    }

    std::string getName(){
        return this->name;
    }

    void setName(std::string newName){
        this->name = newName;
    }
};


#endif //PROJET_POISSON_PLAYER_HPP
