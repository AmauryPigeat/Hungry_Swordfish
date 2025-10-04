#include "welcomeScreen.hpp"

WelcomeScreen::WelcomeScreen(SDL_Renderer* renderer) {
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

    SDL_Surface* startButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "START", FONT::color);
    this->startButtonTextTexture = SDL_CreateTextureFromSurface(renderer, startButtonTextSurface);
    this->buttonStartBackPos = {WINDOW_WIDTH/2 - buttonBackgroundSurface->w/(2*4), this->logoTextPos.y + this->logoTextPos.h + 15, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonStartTextPos = {this->buttonStartBackPos.x + startButtonTextSurface->w / 2 - 5, this->buttonStartBackPos.y + 12, startButtonTextSurface->w, startButtonTextSurface->h};
    SDL_FreeSurface(startButtonTextSurface);

    SDL_Surface* scoreButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "SCORE", FONT::color);
    this->scoreButtonTextTexture = SDL_CreateTextureFromSurface(renderer, scoreButtonTextSurface);
    this->buttonScoreBackPos = {WINDOW_WIDTH/2 - buttonBackgroundSurface->w/(2*4), this->buttonStartBackPos.y + this->buttonStartBackPos.h + 15, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonScoreTextPos = {this->buttonScoreBackPos.x + scoreButtonTextSurface->w / 2 - 5, this->buttonScoreBackPos.y + 12, scoreButtonTextSurface->w, scoreButtonTextSurface->h};
    SDL_FreeSurface(scoreButtonTextSurface);

    SDL_Surface* quitButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "QUIT", FONT::color);
    this->quitButtonTextTexture = SDL_CreateTextureFromSurface(renderer, quitButtonTextSurface);
    this->buttonQuitBackPos = {WINDOW_WIDTH/2 - buttonBackgroundSurface->w/(2*4), this->buttonScoreBackPos.y + this->buttonScoreBackPos.h + 15, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonQuitTextPos = {this->buttonQuitBackPos.x + quitButtonTextSurface->w / 2 + 20, this->buttonQuitBackPos.y + 12, quitButtonTextSurface->w, quitButtonTextSurface->h};
    SDL_FreeSurface(quitButtonTextSurface);

    SDL_FreeSurface(buttonBackgroundSurface);
}

WelcomeScreen::~WelcomeScreen() {
    SDL_DestroyTexture(this->logoTextTexture);
    SDL_DestroyTexture(this->logoImageTexture);

    SDL_DestroyTexture(this->buttonBackgroundTexture);

    SDL_DestroyTexture(this->startButtonTextTexture);
    SDL_DestroyTexture(this->scoreButtonTextTexture);
    SDL_DestroyTexture(this->quitButtonTextTexture);
}

void WelcomeScreen::drawWelcomeScreen(SDL_Renderer *renderer) {
    SDL_RenderCopy(renderer, this->logoTextTexture, NULL, &this->logoTextPos);
    SDL_RenderCopy(renderer, this->logoImageTexture, NULL, &this->logoImagePos);
    // Bouton Start
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonStartBackPos);
    SDL_RenderCopy(renderer, this->startButtonTextTexture, NULL, &this->buttonStartTextPos);

    // Bouton Score
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonScoreBackPos);
    SDL_RenderCopy(renderer, this->scoreButtonTextTexture, NULL, &this->buttonScoreTextPos);

    // Bouton Quit
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonQuitBackPos);
    SDL_RenderCopy(renderer, this->quitButtonTextTexture, NULL, &this->buttonQuitTextPos);
}
