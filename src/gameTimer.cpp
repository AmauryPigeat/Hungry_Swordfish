#include "gameTimer.hpp"

Timer::Timer(SDL_Renderer* renderer){
    this->time = 61;

    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(FONT::font1, std::to_string(this->time - 1).c_str(), FONT::color);
    if (textSurface == NULL) {
        std::cout << "Failed to create text surface: " << TTF_GetError() << std::endl;
    }

   this->timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (this->timerTexture == NULL) {
        std::cout << "Failed to create text texture: " << SDL_GetError() << std::endl;
    }

    this->pos = {WINDOW_WIDTH - textSurface->w, 0, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, this->timerTexture, NULL, &this->pos);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(this->timerTexture);
}

Timer::~Timer() {

}

void Timer::drawTimer(SDL_Renderer *renderer) {
    SDL_Surface* textSurface;
    if(this->time < 10){
        textSurface = TTF_RenderText_Solid(FONT::font1, std::to_string(this->time - 1).c_str(), FONT::color3);
    } else{
        textSurface = TTF_RenderText_Solid(FONT::font1, std::to_string(this->time - 1).c_str(), FONT::color);
    }


    if (textSurface == NULL) {
        std::cout << "Failed to create text surface:" << TTF_GetError() << std::endl;
    }
    this->timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    if (this->timerTexture == NULL) {
        std::cout << "Failed to create text texture: " << SDL_GetError() << std::endl;
    }
    this->pos = {WINDOW_WIDTH - textSurface->w, 0, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, this->timerTexture, NULL, &this->pos);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(this->timerTexture);
}

void Timer::reset() {
    this->time = 64;
}

void Timer::drawStartTimer(SDL_Renderer *renderer) {
    SDL_Surface* startTimerSurface = TTF_RenderText_Solid(FONT::font3, std::to_string(this->time - 60).c_str(), FONT::color3);
    SDL_Texture* startTimerTexture = SDL_CreateTextureFromSurface(renderer, startTimerSurface);
    SDL_Rect startTimerPos = {WINDOW_WIDTH/2 - startTimerSurface->w/2, WINDOW_HEIGHT/2 - startTimerSurface->h/2, startTimerSurface->w, startTimerSurface->h};

    SDL_Rect blurRectangle = {-50, -50, WINDOW_WIDTH+100, WINDOW_HEIGHT+100};

    SDL_RenderFillRect(renderer, &blurRectangle);
    SDL_RenderCopy(renderer, startTimerTexture, NULL, &startTimerPos);

    SDL_FreeSurface(startTimerSurface);
    SDL_DestroyTexture(startTimerTexture);
}
