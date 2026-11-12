#include "screen.hpp"

namespace {

constexpr int logoTop = 30;
constexpr int buttonRowGap = 80;

}  // namespace

Screen::Screen(GameContext& context)
    : context_(context),
      title_(context.renderer, context.assets.mediumFont.get(), config::white,
             "HUNGRY SWORDFISH") {}

void Screen::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        mouse_ = {event.motion.x, event.motion.y};
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        const SDL_Point click{event.button.x, event.button.y};
        for (const ButtonEntry& entry : buttons_) {
            if (entry.button.contains(click)) {
                entry.onClick();
                return;
            }
        }
    }
}

std::optional<ScreenId> Screen::takeRequestedScreen() {
    const std::optional<ScreenId> requested = requestedScreen_;
    requestedScreen_.reset();
    return requested;
}

void Screen::addButton(const std::string& label, SDL_Rect area, std::function<void()> onClick,
                       SDL_Texture* background) {
    if (background == nullptr) {
        background = context_.assets.button.get();
    }
    buttons_.push_back({Button(context_.renderer, background, context_.assets.mediumFont.get(),
                               label, area),
                        std::move(onClick)});
}

void Screen::drawButtons() const {
    for (const ButtonEntry& entry : buttons_) {
        entry.button.draw(entry.button.contains(mouse_));
    }
}

void Screen::drawMenuBackground() const {
    SDL_RenderCopy(context_.renderer, context_.assets.background.get(), nullptr, nullptr);
}

void Screen::dimScreen() const {
    SDL_SetRenderDrawBlendMode(context_.renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(context_.renderer, 0, 0, 0, config::dimOpacity);
    SDL_RenderFillRect(context_.renderer, nullptr);
}

void Screen::drawLogo(int logoSize, int gap) const {
    const SDL_Rect area{screenCenterX - logoSize / 2, logoTop, logoSize, logoSize};
    SDL_RenderCopy(context_.renderer, context_.assets.logo.get(), nullptr, &area);
    title_.drawCentered(screenCenterX, logoTop + logoSize + gap);
}

int Screen::titleBottom(int logoSize, int gap) const {
    return logoTop + logoSize + gap + title_.height();
}

SDL_Rect buttonArea(int centerX, int y) {
    return {centerX - config::buttonWidth / 2, y, config::buttonWidth, config::buttonHeight};
}

std::vector<SDL_Rect> buttonRow(int count, int y) {
    const int step = config::buttonWidth + buttonRowGap;
    const int firstCenter = screenCenterX - step * (count - 1) / 2;

    std::vector<SDL_Rect> areas;
    for (int i = 0; i < count; ++i) {
        areas.push_back(buttonArea(firstCenter + i * step, y));
    }
    return areas;
}
