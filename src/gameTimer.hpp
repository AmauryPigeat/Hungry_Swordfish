#include "constants.hpp"
#include "boidSimulation.hpp"

#ifndef PROJET_POISSON_GAMETIMER_HPP
#define PROJET_POISSON_GAMETIMER_HPP

class Timer {
private:
    __int8 time;
    SDL_Texture* timerTexture;
    SDL_Rect pos;

public:
    Timer(SDL_Renderer* renderer);
    ~Timer();

    int getTime(){
        return this->time;
    }

    void drawTimer(SDL_Renderer* renderer);
    void drawStartTimer(SDL_Renderer* renderer);
    void decrementTimer(){
        this->time--;
    }
    void reset();
};

#endif //PROJET_POISSON_GAMETIMER_HPP
