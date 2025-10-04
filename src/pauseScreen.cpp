#include "pauseScreen.hpp"

PauseScreen::PauseScreen(SDL_Renderer *renderer) {
    SDL_Surface* logoImageSurface = IMG_Load(IMGPATH::logo);
    this->logoImageTexture = SDL_CreateTextureFromSurface(renderer, logoImageSurface);
    this->logoImagePos = {WINDOW_WIDTH/2 - logoImageSurface->w/8, 30, logoImageSurface->w/4, logoImageSurface->h/4};
    SDL_FreeSurface(logoImageSurface);

    SDL_Surface* logoTextSurface = TTF_RenderText_Solid(FONT::font1, "HUNGRY SWORDFISH", FONT::color);
    this->logoTextTexture = SDL_CreateTextureFromSurface(renderer, logoTextSurface);
    this->logoTextPos = {WINDOW_WIDTH/2 - logoTextSurface->w / 2, this->logoImagePos.y + this->logoImagePos.h + 30, logoTextSurface->w, logoTextSurface->h};
    SDL_FreeSurface(logoTextSurface);

    SDL_Surface* buttonBackgroundSurface = IMG_Load(IMGPATH::buttonBackground);
    this->buttonBackgroundTexture = SDL_CreateTextureFromSurface(renderer, buttonBackgroundSurface);

    SDL_Surface* resumeButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "RESUME", FONT::color);
    this->resumeButtonTextTexture = SDL_CreateTextureFromSurface(renderer, resumeButtonTextSurface);
    this->buttonResumeBackPos = {WINDOW_WIDTH/2 - buttonBackgroundSurface->w/4 - 40, this->logoTextPos.y + this->logoTextPos.h + 250, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonResumeTextPos = {this->buttonResumeBackPos.x + resumeButtonTextSurface->w / 2 - 32, this->buttonResumeBackPos.y + 12, resumeButtonTextSurface->w, resumeButtonTextSurface->h};
    SDL_FreeSurface(resumeButtonTextSurface);

    SDL_Surface* quitButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "QUIT", FONT::color);
    this->quitButtonTextTexture = SDL_CreateTextureFromSurface(renderer, quitButtonTextSurface);
    this->buttonQuitBackPos = {WINDOW_WIDTH/2 + 40, this->buttonResumeBackPos.y, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonQuitTextPos = {this->buttonQuitBackPos.x + quitButtonTextSurface->w / 2 + 18, this->buttonQuitBackPos.y + 12, quitButtonTextSurface->w, quitButtonTextSurface->h};
    SDL_FreeSurface(quitButtonTextSurface);

    SDL_Surface* pauseTextSurface = TTF_RenderText_Solid(FONT::font1, "THE GAME IS PAUSED", FONT::color2);
    this->pauseTextTexture = SDL_CreateTextureFromSurface(renderer, pauseTextSurface);
    this->pauseTextPos = {WINDOW_WIDTH/2 - pauseTextSurface->w / 2, this->logoTextPos.y + 100, pauseTextSurface->w, pauseTextSurface->h};
    SDL_FreeSurface(pauseTextSurface);
}

PauseScreen::~PauseScreen() {
    SDL_DestroyTexture(this->logoImageTexture);
    SDL_DestroyTexture(this->logoTextTexture);
    SDL_DestroyTexture(this->buttonBackgroundTexture);
    SDL_DestroyTexture(this->resumeButtonTextTexture);
    SDL_DestroyTexture(this->quitButtonTextTexture);
    SDL_DestroyTexture(this->pauseTextTexture);
}

void PauseScreen::draw(SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, this->logoImageTexture, NULL, &this->logoImagePos);
    SDL_RenderCopy(renderer, this->logoTextTexture, NULL,&this->logoTextPos);

    SDL_RenderCopy(renderer, this->pauseTextTexture, NULL, &this->pauseTextPos);

    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonQuitBackPos);
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonResumeBackPos);

    SDL_RenderCopy(renderer, this->quitButtonTextTexture, NULL, &this->buttonQuitTextPos);
    SDL_RenderCopy(renderer, this->resumeButtonTextTexture, NULL, &this->buttonResumeTextPos);
}


