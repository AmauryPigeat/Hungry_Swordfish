#pragma once

#include "../assets.hpp"
#include "../config.hpp"
#include "../button.hpp"
#include "../gameSession.hpp"
#include "../highScores.hpp"
#include "../text.hpp"

#include <functional>
#include <optional>
#include <string>
#include <vector>

// Les différents écrans du jeu. Quit n'est pas un écran : il ferme le jeu.
enum class ScreenId { Welcome, NameEntry, Playing, Paused, Scores, Quit };

// Ce que tous les écrans partagent.
struct GameContext {
    SDL_Renderer* renderer;
    Assets& assets;
    GameSession& session;
    HighScores& highScores;
};

// Classe de base des écrans. Elle gère ce qu'ils ont en commun :
// les boutons, le logo, et la demande de changement d'écran.
class Screen {
public:
    explicit Screen(GameContext& context);
    virtual ~Screen() = default;

    virtual void onEnter() {}  // appelé quand l'écran devient l'écran courant
    virtual void onLeave() {}  // appelé quand on le quitte
    virtual void handleEvent(const SDL_Event& event);
    virtual void update(float /*dt*/) {}
    virtual void draw() = 0;

    // Écran demandé par goTo() depuis le dernier appel, s'il y en a un.
    std::optional<ScreenId> takeRequestedScreen();

protected:
    void goTo(ScreenId screen) { requestedScreen_ = screen; }

    // background : image de fond du bouton, celle par défaut si nullptr.
    void addButton(const std::string& label, SDL_Rect area, std::function<void()> onClick,
                   SDL_Texture* background = nullptr);
    void drawButtons() const;

    void drawMenuBackground() const;  // image de fond, sans défilement
    void dimScreen() const;           // voile sombre par-dessus ce qui est déjà dessiné

    // Logo carré de côté logoSize en haut de l'écran, avec le titre du jeu dessous.
    void drawLogo(int logoSize, int gap) const;
    int titleBottom(int logoSize, int gap) const;  // ordonnée juste sous le titre

    GameContext& context_;

private:
    struct ButtonEntry {
        Button button;
        std::function<void()> onClick;
    };

    std::vector<ButtonEntry> buttons_;
    Text title_;
    SDL_Point mouse_{-1, -1};
    std::optional<ScreenId> requestedScreen_;
};

// Aides de mise en page.
inline constexpr int screenCenterX = config::windowWidth / 2;

// Bouton centré horizontalement sur centerX, dont le haut est en y.
SDL_Rect buttonArea(int centerX, int y);

// count boutons côte à côte, centrés dans l'écran.
std::vector<SDL_Rect> buttonRow(int count, int y);
