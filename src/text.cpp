#include "text.hpp"

Text::Text(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, const std::string& content)
    : renderer_(renderer), font_(font), color_(color), content_(content) {
    render();
}

void Text::set(const std::string& content) {
    if (content == content_) {
        return;
    }
    content_ = content;
    render();
}

void Text::setColor(SDL_Color color) {
    if (color.r == color_.r && color.g == color_.g && color.b == color_.b && color.a == color_.a) {
        return;
    }
    color_ = color;
    render();
}

void Text::draw(int x, int y) const {
    if (!texture_) {
        return;
    }
    const SDL_Rect destination{x, y, width_, height_};
    SDL_RenderCopy(renderer_, texture_.get(), nullptr, &destination);
}

void Text::drawCentered(int centerX, int y) const {
    draw(centerX - width_ / 2, y);
}

void Text::drawRightAligned(int rightX, int y) const {
    draw(rightX - width_, y);
}

void Text::render() {
    texture_.reset();
    width_ = 0;
    height_ = TTF_FontHeight(font_);
    if (content_.empty()) {
        return;  // SDL_ttf refuse les textes vides, il n'y a simplement rien à dessiner
    }

    const SurfacePtr surface(TTF_RenderUTF8_Solid(font_, content_.c_str(), color_));
    if (!surface) {
        throw SdlError("Rendu du texte \"" + content_ + "\" impossible");
    }
    texture_.reset(SDL_CreateTextureFromSurface(renderer_, surface.get()));
    if (!texture_) {
        throw SdlError("Création de la texture du texte impossible");
    }
    width_ = surface->w;
    height_ = surface->h;
}
