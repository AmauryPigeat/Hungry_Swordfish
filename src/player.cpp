#include "player.hpp"

Player::Player(SDL_Renderer *renderer){
    this->pos.x = WINDOW_WIDTH/2;
    this->pos.y = WINDOW_HEIGHT/2;
    this->pos.w = this->sizeW;
    this->pos.h = this->sizeH;
    this->playerGifIter = 0;

    this->scorePoint = 0;
    this->killCounter = 0;

    SDL_Surface *playerSurface;

    for(unsigned int i = 0;i < 4;i++){
        playerSurface = IMG_Load(IMGPATH::playerWalk[i]);
        if (playerSurface == NULL) {
            std::cout << "Erreur de création de surface player" << SDL_GetError() << std::endl;
        }
        this->playerTextureWalk[i] = SDL_CreateTextureFromSurface(renderer, playerSurface);
        if(this->playerTextureWalk[i] == NULL) {
            std::cout << "Erreur de création de texture playerWalk" << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(playerSurface);
    }

    for(unsigned int i = 0;i < 4;i++){
        playerSurface = IMG_Load(IMGPATH::playerIdle[i]);
        if (playerSurface == NULL) {
            std::cout << "Erreur de création de surface player" << SDL_GetError() << std::endl;
        }
        this->playerTextureIdle[i] = SDL_CreateTextureFromSurface(renderer, playerSurface);
        if(this->playerTextureIdle[i] == NULL) {
            std::cout << "Erreur de création de texture playerWalk" << SDL_GetError() << std::endl;
        }
        SDL_FreeSurface(playerSurface);
    }

    SDL_Surface* mainTextSurface = TTF_RenderText_Solid(FONT::font1, "Type the player's name", FONT::color);
    this->mainTextTexture = SDL_CreateTextureFromSurface(renderer, mainTextSurface);
    this->mainTextPos = {WINDOW_WIDTH/2 - mainTextSurface->w/2, 250, mainTextSurface->w, mainTextSurface->h};
    SDL_FreeSurface(mainTextSurface);
    SDL_Surface* secondaryTextSurface = TTF_RenderText_Solid(FONT::font2, "Max 16 characters | No special characters | HAVE FUN", FONT::color2);
    this->secondaryTextTexture = SDL_CreateTextureFromSurface(renderer, secondaryTextSurface);
    this->secondaryTextPos = {WINDOW_WIDTH/2 - secondaryTextSurface->w/2, this->mainTextPos.y + this->mainTextPos.h + 100, secondaryTextSurface->w, secondaryTextSurface->h};
    SDL_FreeSurface(secondaryTextSurface);
}

Player::~Player(){
    for(SDL_Texture *i : this->playerTextureWalk){
        SDL_DestroyTexture(i);
    }
    for(SDL_Texture *i : this->playerTextureIdle){
        SDL_DestroyTexture(i);
    }
    SDL_DestroyTexture(this->mainTextTexture);
    SDL_DestroyTexture(this->secondaryTextTexture);
}

void Player::drawPlayer(SDL_Renderer *renderer, int angle, bool moving){
    this->playerGifIter++;
    this->playerGifIter %= 16;
    SDL_RendererFlip flip;
    if(angle > 90){
        flip = SDL_FLIP_HORIZONTAL;
        angle = 0;
    }
    else{
        flip = SDL_FLIP_NONE;
    }
    if(moving){
        SDL_RenderCopyEx(renderer, this->playerTextureWalk[this->playerGifIter/4],  NULL, &this->pos, angle, NULL, flip);
    } // animation du personnage différente en fonction de si il se déplace ou pas
    else{
        SDL_RenderCopyEx(renderer, this->playerTextureIdle[this->playerGifIter/4],  NULL, &this->pos, angle, NULL, flip);
    }
}

void Player::checkEveryColisions(BoidSimulation *simulation) {
    SDL_Rect playerPos = SDL_Rect(simulation->camera.x + this->pos.x, simulation->camera.y + this->pos.y, this->sizeW, this->sizeH);
    for (auto it = simulation->boids.begin(); it != simulation->boids.end(); ) {
        if (SDL_HasIntersection(&playerPos, &it->pos)){
            this->killCounter++;
            this->scorePoint += it->type + 1;
            // Supprime l'élément et met à jour l'itérateur
            it = simulation->boids.erase(it); // renvoie l'élément suivant
        } else {
            ++it; // Passe à l'élément suivant si pas de suppression
        }
    }
}

void Player::reset() {
    this->playerGifIter = 0;
    this->scorePoint = 0;
    this->killCounter = 0;
    this->pos.x = WINDOW_WIDTH/2;
    this->pos.y = WINDOW_HEIGHT/2;
    this->name = "";
}

void Player::drawAskName(SDL_Renderer *renderer, std::string tempName) {
    SDL_RenderCopy(renderer, this->mainTextTexture, NULL, &this->mainTextPos);
    SDL_RenderCopy(renderer, this->secondaryTextTexture, NULL, &this->secondaryTextPos);
    if(tempName.size() < 16) {
        tempName += "_";
    }

    SDL_Surface* playerNameTypingSurface = TTF_RenderText_Solid(FONT::font1, tempName.c_str(), FONT::color);
    this->playerNameTypingTexture = SDL_CreateTextureFromSurface(renderer, playerNameTypingSurface);
    this->playerNameTypingPos = {WINDOW_WIDTH/2 - playerNameTypingSurface->w/2, this->mainTextPos.y + mainTextPos.h + 50, playerNameTypingSurface->w, playerNameTypingSurface->h};
    SDL_FreeSurface(playerNameTypingSurface);
    SDL_RenderCopy(renderer, this->playerNameTypingTexture, NULL, &this->playerNameTypingPos);
    SDL_DestroyTexture(this->playerNameTypingTexture);
}

