#include "screens.hpp"

namespace {

constexpr int promptY = 250;
constexpr int lineSpacing = 50;

// Lettres, chiffres et ponctuation ASCII. Le ';' est exclu car il sert
// de séparateur dans le fichier des scores.
bool isAllowedInName(char character) {
    return character >= ' ' && character <= '~' && character != ';';
}

}  // namespace

NameEntryScreen::NameEntryScreen(GameContext& context)
    : Screen(context),
      prompt_(context.renderer, context.assets.mediumFont.get(), config::white,
              "Type the player's name"),
      typedName_(context.renderer, context.assets.mediumFont.get(), config::white),
      hint_(context.renderer, context.assets.smallFont.get(), config::mint,
            "Max 16 characters | ENTER to play | ESC to go back") {}

void NameEntryScreen::onEnter() {
    // Le dernier pseudo utilisé est proposé par défaut.
    name_ = context_.session.playerName();
    SDL_StartTextInput();
}

void NameEntryScreen::onLeave() {
    SDL_StopTextInput();
}

void NameEntryScreen::handleEvent(const SDL_Event& event) {
    Screen::handleEvent(event);

    if (event.type == SDL_TEXTINPUT) {
        for (const char* character = event.text.text; *character != '\0'; ++character) {
            if (name_.size() < config::maxNameLength && isAllowedInName(*character)) {
                name_ += *character;
            }
        }
    } else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_BACKSPACE:
                if (!name_.empty()) {
                    name_.pop_back();
                }
                break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                if (!name_.empty()) {
                    context_.session.start(name_);
                    goTo(ScreenId::Playing);
                }
                break;
            case SDLK_ESCAPE:
                goTo(ScreenId::Welcome);
                break;
            default:
                break;
        }
    }
}

void NameEntryScreen::draw() {
    drawMenuBackground();

    // Le '_' sert de curseur tant qu'il reste de la place.
    if (name_.size() < config::maxNameLength) {
        typedName_.set(name_ + "_");
    } else {
        typedName_.set(name_);
    }

    prompt_.drawCentered(screenCenterX, promptY);
    const int nameY = promptY + prompt_.height() + lineSpacing;
    typedName_.drawCentered(screenCenterX, nameY);
    hint_.drawCentered(screenCenterX, nameY + typedName_.height() + lineSpacing);
}
