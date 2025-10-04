#ifndef PROJET_POISSON_GAME_HPP
#define PROJET_POISSON_GAME_HPP

#include "player.hpp"
#include "boidSimulation.hpp"
#include "gameTimer.hpp"
#include "scoreboard.hpp"
#include "welcomeScreen.hpp"
#include "endScreen.hpp"
#include "pauseScreen.hpp"
#include "scoreScreen.hpp"

#include <thread>

void checkDeplacement(SDL_Renderer* renderer, BoidSimulation* simulation, Player* player, SDL_Event event, bool* moving, int* angle);

void checkClick(WelcomeScreen* welcome, SDL_Event event, int* runningType, EndScreen* finish, PauseScreen* pause, ScoreScreen* score);


#endif //PROJET_POISSON_GAME_HPP
