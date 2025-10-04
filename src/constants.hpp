#ifndef PROJET_POISSON_CONSTANTS_HPP
#define PROJET_POISSON_CONSTANTS_HPP

#include<iostream>
#include<vector>
#include<cmath>
#include<cstdlib>
#include<ctime>
#include<algorithm>
#include<thread>
#include<random>
#include<fstream>
#include<filesystem>
#include<sstream>
#include<cstdio>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

// Taille de la fenêtre
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// Taille de l'environement
const int ENV_WIDTH = 1920*2;
const int ENV_HEIGHT = 1080*2;

const int FRAME_DURATION = 7; // 60FPS = 16 144FPS = 7ms
const int UPDATE_DELAY = 16; // On update les poissons en équivalent 60 FPS
/* TODO : Trouver une solution pour pouvoir avoir une délai de checkDeplacement plus faible que les Frames
 * Surement refonte du main obligatoire (en plus il est bad long)
 */


// Taille de la caméra
const int CAMERA_WIDTH = 320;
const int CAMERA_HEIGHT = 180;
const int CAMERA_SPEED = 10;

/*
 * Si on veut changer le nombre de FPS : FRAME_DURATION ~= 1000/60 (avec un int)
 * Le changement de FPS demanderait une adaptation de la gestion des poissons, car ils s'update à chaque frame
 * Pour l'instant, 60 FPS c'est très bien
 */


namespace IMGPATH {
    inline const char* background = "../img/background_image_4k.png";
    inline const char* logo = "../img/swordfish_game_logo.png";
    inline const char* buttonBackground = "../img/button_background.png";
    inline const char* buttonDeleteBackground = "../img/button_delete_background.png";
    inline const char* fishWalk[3][5] = {
            {"../img/Walk_fish_1/Walk_fish-1.png",
            "../img/Walk_fish_1/Walk_fish-2.png",
            "../img/Walk_fish_1/Walk_fish-3.png",
            "",
            ""},
            {"../img/Walk_fish_2/Walk_fish-1.png",
             "../img/Walk_fish_2/Walk_fish-2.png",
             "../img/Walk_fish_2/Walk_fish-3.png",
             "../img/Walk_fish_2/Walk_fish-4.png",
             ""},
            {"../img/Walk_fish_3/Walk_fish-1.png",
             "../img/Walk_fish_3/Walk_fish-2.png",
             "../img/Walk_fish_3/Walk_fish-3.png",
             "../img/Walk_fish_3/Walk_fish-4.png",
             "../img/Walk_fish_3/Walk_fish-5.png"}
    };
    inline const char* playerWalk[4] = {
            "../img/Walk_player/Walk_player-1.png",
            "../img/Walk_player/Walk_player-2.png",
            "../img/Walk_player/Walk_player-3.png",
            "../img/Walk_player/Walk_player-4.png"
    };
    inline const char* playerIdle[4] = {
            "../img/Idle_player/Idle_player-1.png",
            "../img/Idle_player/Idle_player-2.png",
            "../img/Idle_player/Idle_player-3.png",
            "../img/Idle_player/Idle_player-4.png"
    };
}

namespace FONT {
    // Pas de const sinon refus de SDL2, variables générales utilisées dans la DA du jeu
    extern TTF_Font *font1;
    extern TTF_Font *font2;
    extern TTF_Font *font3;
    extern SDL_Color color;
    extern SDL_Color color2;
    extern SDL_Color color3;
}
void initFonts();
#endif //PROJET_POISSON_CONSTANTS_HPP
