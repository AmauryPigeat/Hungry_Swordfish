#include "button.hpp"

#include "config.hpp"

Button::Button(SDL_Renderer* renderer, SDL_Texture* background, TTF_Font* font,
               const std::string& label, SDL_Rect area)
    : renderer_(renderer),
      background_(background),
      area_(area),
      label_(renderer, font, config::white, label) {}

void Button::draw(bool hovered) const {
    // Le fond est légèrement assombri tant que la souris n'est pas dessus.
    const Uint8 brightness = hovered ? 255 : 215;
    SDL_SetTextureColorMod(background_, brightness, brightness, brightness);
    SDL_RenderCopy(renderer_, background_, nullptr, &area_);
    SDL_SetTextureColorMod(background_, 255, 255, 255);

    label_.drawCentered(area_.x + area_.w / 2, area_.y + (area_.h - label_.height()) / 2);
}
