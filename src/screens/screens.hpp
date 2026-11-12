#pragma once

#include "screen.hpp"

#include <string>
#include <vector>

// Écran d'accueil : START / SCORE / QUIT.
class WelcomeScreen : public Screen {
public:
    explicit WelcomeScreen(GameContext& context);
    void draw() override;
};

// Saisie du pseudo avant de lancer une partie.
class NameEntryScreen : public Screen {
public:
    explicit NameEntryScreen(GameContext& context);
    void onEnter() override;
    void onLeave() override;
    void handleEvent(const SDL_Event& event) override;
    void draw() override;

private:
    std::string name_;
    Text prompt_;
    Text typedName_;
    Text hint_;
};

// La partie en cours, avec le score et le chrono par-dessus.
class PlayingScreen : public Screen {
public:
    explicit PlayingScreen(GameContext& context);
    void handleEvent(const SDL_Event& event) override;
    void update(float dt) override;
    void draw() override;

private:
    Text kills_;
    Text score_;
    Text timer_;
    Text countdown_;
};

// Pause : la partie reste visible, figée, derrière le menu.
class PausedScreen : public Screen {
public:
    explicit PausedScreen(GameContext& context);
    void handleEvent(const SDL_Event& event) override;
    void draw() override;

private:
    Text message_;
};

// Résultat de la partie qui vient de se terminer.
class GameOverScreen : public Screen {
public:
    explicit GameOverScreen(GameContext& context);
    void onEnter() override;
    void draw() override;

private:
    Text score_;
    Text kills_;
};

// Les dernières parties jouées.
class ScoresScreen : public Screen {
public:
    explicit ScoresScreen(GameContext& context);
    void onEnter() override;
    void draw() override;

private:
    void refreshLines();

    Text heading_;
    Text emptyMessage_;
    std::vector<Text> lines_;
};
