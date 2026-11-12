#pragma once

#include "assets.hpp"
#include "gameSession.hpp"
#include "highScores.hpp"
#include "screens/screen.hpp"
#include "sdlUtils.hpp"

#include <array>
#include <memory>

// Le programme : la fenêtre, les ressources, les écrans et la boucle principale.
class Game {
public:
    Game();

    // Tourne jusqu'à la fermeture du jeu.
    void run();

    // Une image : évènements, mise à jour, affichage.
    void runFrame();

    bool isRunning() const { return running_; }
    SDL_Renderer* renderer() const { return renderer_.get(); }

private:
    static constexpr std::size_t screenCount = static_cast<std::size_t>(ScreenId::Quit);

    Screen& currentScreen() { return *screens_[static_cast<std::size_t>(current_)]; }
    void handleEvents();
    void applyScreenRequest();
    void toggleFullscreen();

    // L'ordre des membres compte : ils sont détruits dans l'ordre inverse,
    // donc les textures avant le renderer, et SDL en tout dernier.
    SdlContext sdl_;
    WindowPtr window_;
    RendererPtr renderer_;
    Assets assets_;
    GameSession session_;
    HighScores highScores_;
    GameContext context_;
    std::array<std::unique_ptr<Screen>, screenCount> screens_;

    ScreenId current_ = ScreenId::Welcome;
    bool running_ = true;
    Uint64 lastFrameTime_ = 0;
    float timeToSimulate_ = 0.0f;  // temps écoulé pas encore traité par update()
};
