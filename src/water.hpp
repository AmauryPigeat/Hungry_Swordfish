#pragma once

#include "assets.hpp"
#include "camera.hpp"

// Fond d'eau animé : les images de img/Water/ collées côte à côte, comme du carrelage.
//
// Remplit le rectangle (0, 0, width, height) du monde, vu par camera.
// Seules les tuiles visibles à l'écran sont dessinées : le coût dépend de la taille
// de l'écran, pas de celle du monde.
// Si width ou height n'est pas un multiple de config::waterTileSize, la dernière
// colonne et la dernière ligne sont coupées pour ne pas dépasser.
void drawWater(SDL_Renderer* renderer, const Animation& water, const Camera& camera, float width,
               float height);
