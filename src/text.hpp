#pragma once

#include "sdlUtils.hpp"

#include <string>

// Un texte affiché à l'écran.
// La texture n'est recalculée que si le contenu ou la couleur change :
// on peut appeler set() à chaque image sans rien coûter.
class Text {
public:
    Text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const std::string& content = "");

    void set(const std::string& content);
    void setColor(SDL_Color color);

    int width() const { return width_; }
    int height() const { return height_; }

    void draw(int x, int y) const;
    void drawCentered(int centerX, int y) const;  // centré horizontalement
    void drawRightAligned(int rightX, int y) const;

    // Centré sur (centerX, centerY), agrandi ou rétréci par scale (texte dans le monde zoomé).
    void drawCenteredScaled(float centerX, float centerY, float scale) const;

private:
    void render();

    SDL_Renderer* renderer_;
    TTF_Font* font_;
    SDL_Color color_;
    std::string content_;
    TexturePtr texture_;
    int width_ = 0;
    int height_ = 0;
};
