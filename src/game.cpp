#include "game.hpp"

void checkDeplacement(SDL_Renderer* renderer, BoidSimulation* simulation, Player* player, SDL_Event event, bool* moving, int* angle){
    switch (event.key.keysym.sym) {
        case SDLK_UP:
            if(player->pos.y - CAMERA_SPEED*2 <= 0){
                player->pos.y = 0;
            }

            if((simulation->camera.y == 0 && player->pos.y - CAMERA_SPEED*2 > 0) || (simulation->camera.y == ENV_HEIGHT - CAMERA_HEIGHT && player->pos.y > WINDOW_HEIGHT/2)){
                player->pos.y -= CAMERA_SPEED*2;
            }
            else if(simulation->camera.y - CAMERA_SPEED > 0){
                simulation->camera.y -= CAMERA_SPEED;
                player->pos.y = WINDOW_HEIGHT/2;
            }
            else{
                simulation->camera.y = 0;
            }
            // Gestion de l'angle de déplacement
            if(player->pos.y > 0){
                *angle = -90;
            } else{
                *angle = 0;
            }
            *moving = true;
            break;
        case SDLK_DOWN:
            // Déplacer la caméra vers le bas
            if(player->pos.y + CAMERA_SPEED*2 >= WINDOW_HEIGHT){
                player->pos.y = WINDOW_HEIGHT - player->pos.h;
            }

            if((player->pos.y < WINDOW_HEIGHT/2 && simulation->camera.y == 0) || (simulation->camera.y == ENV_HEIGHT - CAMERA_HEIGHT && player->pos.y != WINDOW_HEIGHT - player->pos.h)){
                player->pos.y += CAMERA_SPEED*2;
            }
            else if(simulation->camera.y + CAMERA_HEIGHT + CAMERA_SPEED < ENV_HEIGHT) { // limite bas
                simulation->camera.y += CAMERA_SPEED;
                player->pos.y = WINDOW_HEIGHT/2;
            }
            else{
                simulation->camera.y = ENV_HEIGHT - CAMERA_HEIGHT;
                if(player->pos.y + CAMERA_SPEED*2 < WINDOW_HEIGHT - player->pos.h) {
                    player->pos.y += CAMERA_SPEED*2;
                }
            }

            if(player->pos.y == WINDOW_HEIGHT - player->pos.h){
                *angle = 0;
            }
            else{
                *angle = 90;
            }
            *moving = true;
            break;
        case SDLK_LEFT:
            if(player->pos.x - CAMERA_SPEED*2 <= 0){
                player->pos.x = 0;
            }
            if(simulation->camera.x == 0 || (simulation->camera.x == ENV_WIDTH - CAMERA_WIDTH && player->pos.x > WINDOW_WIDTH/2)){
                player->pos.x -= CAMERA_SPEED*2;
            }
            else if(simulation->camera.x - CAMERA_SPEED > 0){
                simulation->camera.x -= CAMERA_SPEED;
                player->pos.x = WINDOW_WIDTH/2;
            }
            else{
                simulation->camera.x = 0;
            }
            if(player->pos.x > 0){
                *angle = 180;
            }
            else{
                *angle = 0;
            }
            *moving = true;
            break;
        case SDLK_RIGHT:
            *angle = 0; // la position d'origine du poisson regarde à droite, il n'y a pas d'angle à gérer ici
            if(player->pos.x + CAMERA_SPEED*2 >= WINDOW_WIDTH - player->pos.w){
                player->pos.x = WINDOW_WIDTH - player->pos.w;
            }

            if((simulation->camera.x == 0 && player->pos.x < WINDOW_WIDTH/2) || (simulation->camera.x == ENV_WIDTH - CAMERA_WIDTH && player->pos.x + CAMERA_SPEED*2 <= WINDOW_WIDTH - player->pos.w)){
                player->pos.x += CAMERA_SPEED*2;
            }
            else if(simulation->camera.x + CAMERA_SPEED < ENV_WIDTH - CAMERA_WIDTH){
                simulation->camera.x += CAMERA_SPEED;
                player->pos.x = WINDOW_WIDTH/2;
            }
            else{
                simulation->camera.x = ENV_WIDTH - CAMERA_WIDTH;
            }
            *moving = true;
            break;
    }
}

void checkClick(welcomeScreen* welcome, SDL_Event event, int* runningType, endScreen* finish, pauseScreen* pause, scoreScreen* score){
    int x = event.button.x; // Position X du clic
    int y = event.button.y;
    // Check bouton start
    if(*runningType == 0) {
        if (x >= welcome->buttonStartBackPos.x && x <= welcome->buttonStartBackPos.x + welcome->buttonStartBackPos.w) {
            if (y >= welcome->buttonStartBackPos.y &&
                y <= welcome->buttonStartBackPos.y + welcome->buttonStartBackPos.h) {
                *runningType = 1;
                return;
            }
        }
        if (x >= welcome->buttonScoreBackPos.x && x <= welcome->buttonScoreBackPos.x + welcome->buttonScoreBackPos.w) {
            if (y >= welcome->buttonScoreBackPos.y &&
                y <= welcome->buttonScoreBackPos.y + welcome->buttonScoreBackPos.h) {
                *runningType = 4;
                return;
            }
        }
        if (x >= welcome->buttonQuitBackPos.x && x <= welcome->buttonQuitBackPos.x + welcome->buttonQuitBackPos.w) {
            if (y >= welcome->buttonQuitBackPos.y && y <= welcome->buttonQuitBackPos.y + welcome->buttonQuitBackPos.h) {
                *runningType = -1;
                return;
            }
        }
    }
    if(*runningType == 2){
        if(x >= finish->buttonReStartBackPos.x && x <= finish->buttonReStartBackPos.x + finish->buttonReStartBackPos.w){
            if(y >= finish->buttonReStartBackPos.y && y <= finish->buttonReStartBackPos.y + finish->buttonReStartBackPos.h){
                *runningType = 1;
                return ;
            }
        }
        if(x >= finish->buttonHomeBackPos.x && x <= finish->buttonHomeBackPos.x + finish->buttonHomeBackPos.w){
            if(y >= finish->buttonHomeBackPos.y && y <= finish->buttonHomeBackPos.y + finish->buttonHomeBackPos.h){
                *runningType = 0;
            }
        }
        if(x >= finish->buttonQuitBackPos.x && x <= finish->buttonQuitBackPos.x + finish->buttonQuitBackPos.w){
            if(y >= finish->buttonHomeBackPos.y && y <= finish->buttonQuitBackPos.y + finish->buttonQuitBackPos.h){
                *runningType = -1;
            }
        }
    }
    if(*runningType == 3){
        if(x >= pause->buttonQuitBackPos.x && x <= pause->buttonQuitBackPos.x + pause->buttonQuitBackPos.w){
            if(y >= pause->buttonQuitBackPos.y && y <= pause->buttonQuitBackPos.y + pause->buttonQuitBackPos.h){
                *runningType = -1;
                return ;
            }
        }
        if(x >= pause->buttonResumeBackPos.x && x <= pause->buttonResumeBackPos.x + pause->buttonResumeBackPos.w){
            if(y >= pause->buttonResumeBackPos.y && y <= pause->buttonResumeBackPos.y + pause->buttonResumeBackPos.h){
                *runningType = 1;
                return ;
            }
        }
    }
    if(*runningType == 4){
        if(x >= score->buttonHomeBackPos.x && x <= score->buttonHomeBackPos.x + score->buttonHomeBackPos.w){
            if(y >= score->buttonHomeBackPos.y && y <= score->buttonHomeBackPos.y + score->buttonHomeBackPos.h){
                *runningType = 0;
                return ;
            }
        }
        if(x >= score->buttonDeleteBackPos.x && x <= score->buttonDeleteBackPos.x + score->buttonDeleteBackPos.w){
            if(y >= score->buttonDeleteBackPos.y && y <= score->buttonDeleteBackPos.y + score->buttonDeleteBackPos.h){
                score->deleteScore();
                return ;
            }
        }
        if(x >= score->buttonQuitBackPos.x && x <= score->buttonQuitBackPos.x + score->buttonQuitBackPos.w){
            if(y >= score->buttonQuitBackPos.y && y <= score->buttonQuitBackPos.y + score->buttonQuitBackPos.w){
                *runningType = -1;
                return ;
            }
        }
    }
}

