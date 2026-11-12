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
    void onEnter() override { dashPressed_ = false; }  // pas de dash fantôme après une pause
    void handleEvent(const SDL_Event& event) override;
    void update(float dt) override;
    void draw() override;

private:
    void drawDashGauge() const;

    bool dashPressed_ = false;  // appui sur Espace pas encore transmis à la partie
    Text kills_;
    Text score_;
    Text timer_;
    Text countdown_;
    Text sprintEffect_;
    Text doubleScoreEffect_;
    Text dashLabel_;
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

// Classement des meilleures parties, en tableau.
class ScoresScreen : public Screen {
public:
    explicit ScoresScreen(GameContext& context);
    void onEnter() override;
    void draw() override;

private:
    // Une ligne du tableau : un texte par colonne, pour pouvoir les aligner.
    struct Row {
        Text rank;
        Text name;
        Text score;
        Text kills;
    };

    void refreshRows();
    void drawRow(const Row& row, int y) const;

    Text heading_;
    Text emptyMessage_;
    Row header_;
    std::vector<Row> rows_;
};
