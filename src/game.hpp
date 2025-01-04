#ifndef PROJET_POISSON_GAME_HPP
#define PROJET_POISSON_GAME_HPP

#include "player.hpp"
#include "boidSimulation.hpp"
#include "gameTimer.hpp"
#include "Scoreboard.hpp"
#include "welcomeScreen.hpp"
#include "endScreen.hpp"
#include "pauseScreen.hpp"
#include "scoreScreen.hpp"

void checkDeplacement(SDL_Renderer* renderer, BoidSimulation* simulation, Player* player, SDL_Event event, bool* moving, int* angle);

void checkClick(welcomeScreen* welcome, SDL_Event event, int* runningType, endScreen* finish, pauseScreen* pause, scoreScreen* score);

#endif //PROJET_POISSON_GAME_HPP
