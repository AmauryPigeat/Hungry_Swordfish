#pragma once

#include "text.hpp"

#include <string>

// Bouton cliquable : une image de fond et un libellé centré dessus.
class Button {
public:
    Button(SDL_Renderer* renderer, SDL_Texture* background, TTF_Font* font,
           const std::string& label, SDL_Rect area);

    bool contains(SDL_Point point) const { return SDL_PointInRect(&point, &area_) == SDL_TRUE; }
    void draw(bool hovered) const;

private:
    SDL_Renderer* renderer_;
    SDL_Texture* background_;
    SDL_Rect area_;
    Text label_;
};
