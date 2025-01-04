#include "Scoreboard.hpp"

using namespace std;

Scoreboard::Scoreboard(SDL_Renderer* renderer) {
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(FONT::font1, "Score : 0", FONT::color);
    if (scoreSurface == NULL) {
        std::cout << "Failed to create score surface: " << TTF_GetError() << std::endl;
    }

    this->scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    if (this->scoreTexture == NULL) {
        std::cout << "Failed to create score texture: " << SDL_GetError() << std::endl;
    }

    this->killCountPos = {0, 0, scoreSurface->w, scoreSurface->h};

    SDL_Surface* killCountSurface = TTF_RenderText_Solid(FONT::font1, "Nombre de kill : 0", FONT::color);
    if (killCountSurface == NULL) {
        std::cout << "Failed to create killCount surface: " << TTF_GetError() << std::endl;
    }

    this->killCountTexture = SDL_CreateTextureFromSurface(renderer, killCountSurface);
    if (this->killCountTexture == NULL) {
        std::cout << "Failed to create killCount texture: " << SDL_GetError() << std::endl;
    }
    this->scorePos = {0, this->killCountPos.h, killCountSurface->w, killCountSurface->h};
    SDL_RenderCopy(renderer, this->scoreTexture, NULL, &this->scorePos);
    SDL_RenderCopy(renderer, this->killCountTexture, NULL, &this->killCountPos);

    SDL_FreeSurface(scoreSurface);
    SDL_FreeSurface(killCountSurface);
    SDL_DestroyTexture(this->scoreTexture);
    SDL_DestroyTexture(this->killCountTexture);
}

Scoreboard::~Scoreboard() {
}

void Scoreboard::drawPlayerScore(SDL_Renderer *renderer, Player *player) {
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(FONT::font1, ("Score : " + std::to_string(player->getScorePoint())).c_str(), FONT::color);
    if (scoreSurface == NULL){
        std::cout << "Failed to create score surface: " << TTF_GetError() << std::endl;
    }
    this->scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);

    if (this->scoreTexture == NULL){
        std::cout << "Failed to create score texture: " << SDL_GetError() << std::endl;
    }

    this->scorePos.w = scoreSurface->w;

    SDL_RenderCopy(renderer, this->scoreTexture, NULL, &this->scorePos);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);
}

void Scoreboard::drawPlayerKillCount(SDL_Renderer *renderer, Player *player) {
    SDL_Surface* killCountSurface = TTF_RenderText_Solid(FONT::font1, ("Nombres de kill : " + std::to_string(player->getKillCount())).c_str(), FONT::color);
    if (killCountSurface == NULL){
        std::cout << "Failed to create score surface: " << TTF_GetError() << std::endl;
    }
    this->killCountTexture = SDL_CreateTextureFromSurface(renderer, killCountSurface);

    if (this->killCountTexture == NULL){
        std::cout << "Failed to create score texture: " << SDL_GetError() << std::endl;
    }

    this->killCountPos.w = killCountSurface->w;

    SDL_RenderCopy(renderer, this->killCountTexture, NULL, &this->killCountPos);
    SDL_FreeSurface(killCountSurface);
    SDL_DestroyTexture(this->killCountTexture);
}


