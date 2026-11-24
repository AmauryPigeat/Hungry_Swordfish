#include "game.hpp"

#include "config.hpp"
#include "screens/screens.hpp"

#include <algorithm>

namespace {

WindowPtr createWindow() {
    WindowPtr window(SDL_CreateWindow(config::windowTitle, SDL_WINDOWPOS_CENTERED,
                                      SDL_WINDOWPOS_CENTERED, config::windowWidth,
                                      config::windowHeight, SDL_WINDOW_RESIZABLE));
    if (!window) {
        throw SdlError("Création de la fenêtre impossible");
    }
    return window;
}

RendererPtr createRenderer(SDL_Window* window) {
    // VSync : l'affichage suit l'écran (60, 144 Hz...) sans réglage à faire.
    // SDL choisit de lui-même le meilleur renderer disponible (accéléré si possible).
    RendererPtr renderer(SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC));
    if (!renderer) {
        renderer.reset(SDL_CreateRenderer(window, -1, 0));  // dernier recours, sans VSync
    }
    if (!renderer) {
        throw SdlError("Création du renderer impossible");
    }
    // Le jeu dessine toujours en 1280x720, SDL adapte à la taille réelle de la fenêtre.
    SDL_RenderSetLogicalSize(renderer.get(), config::windowWidth, config::windowHeight);
    return renderer;
}

}  // namespace

Game::Game()
    : window_(createWindow()),
      renderer_(createRenderer(window_.get())),
      assets_(renderer_.get()),
      session_(renderer_.get(), assets_),
      highScores_(executableDirectory() / config::scoreFileName),
      context_{renderer_.get(), assets_, session_, highScores_} {
    const auto addScreen = [this](ScreenId id, std::unique_ptr<Screen> screen) {
        screens_[static_cast<std::size_t>(id)] = std::move(screen);
    };
    addScreen(ScreenId::Welcome, std::make_unique<WelcomeScreen>(context_));
    addScreen(ScreenId::NameEntry, std::make_unique<NameEntryScreen>(context_));
    addScreen(ScreenId::Playing, std::make_unique<PlayingScreen>(context_));
    addScreen(ScreenId::Paused, std::make_unique<PausedScreen>(context_));
    addScreen(ScreenId::Scores, std::make_unique<ScoresScreen>(context_));

    SDL_StopTextInput();  // activé par défaut sur PC, seul l'écran du pseudo en a besoin
    currentScreen().onEnter();
    lastFrameTime_ = SDL_GetPerformanceCounter();
}

void Game::run() {
    while (running_) {
        runFrame();
    }
}

void Game::runFrame() {
    // Temps écoulé depuis l'image précédente.
    const Uint64 now = SDL_GetPerformanceCounter();
    const float elapsed = static_cast<float>(now - lastFrameTime_) /
                          static_cast<float>(SDL_GetPerformanceFrequency());
    lastFrameTime_ = now;

    handleEvents();

    // La logique avance par pas fixes de tickDuration, autant de fois que nécessaire
    // pour rattraper le temps réel : le jeu va à la même vitesse à 30 comme à 240 FPS.
    // Le plafond évite de simuler d'un coup une longue interruption (fenêtre déplacée...).
    timeToSimulate_ = std::min(timeToSimulate_ + elapsed, 0.25f);
    while (running_ && timeToSimulate_ >= config::tickDuration) {
        currentScreen().update(config::tickDuration);
        applyScreenRequest();
        timeToSimulate_ -= config::tickDuration;
    }

    if (running_) {
        SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, 255);
        SDL_RenderClear(renderer_.get());
        currentScreen().draw();
        SDL_RenderPresent(renderer_.get());
    }

    // Laisse souffler le processeur si la VSync n'est pas disponible.
    SDL_Delay(1);
}

void Game::handleEvents() {
    SDL_Event event;
    while (running_ && SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running_ = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11 &&
                   event.key.repeat == 0) {
            toggleFullscreen();
        } else {
            currentScreen().handleEvent(event);
            applyScreenRequest();
        }
    }
}

void Game::applyScreenRequest() {
    const std::optional<ScreenId> requested = currentScreen().takeRequestedScreen();
    if (!requested) {
        return;
    }

    currentScreen().onLeave();
    if (*requested == ScreenId::Quit) {
        running_ = false;
        return;
    }
    current_ = *requested;
    currentScreen().onEnter();
}

void Game::toggleFullscreen() {
    const bool fullscreen = (SDL_GetWindowFlags(window_.get()) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
    if (fullscreen) {
        SDL_SetWindowFullscreen(window_.get(), 0);  // retour en mode fenêtré
    } else {
        SDL_SetWindowFullscreen(window_.get(), SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
}
