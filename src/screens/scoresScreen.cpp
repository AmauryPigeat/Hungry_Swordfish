#include "screens.hpp"

#include <algorithm>

namespace {

constexpr int logoSize = 162;
constexpr int logoGap = 10;
constexpr int lineSpacing = 8;

}  // namespace

ScoresScreen::ScoresScreen(GameContext& context)
    : Screen(context),
      heading_(context.renderer, context.assets.mediumFont.get(), config::white,
               "Last " + std::to_string(config::displayedScores) + " scores"),
      emptyMessage_(context.renderer, context.assets.smallFont.get(), config::mint,
                    "No game played yet") {
    const std::vector<SDL_Rect> areas = buttonRow(3, titleBottom(logoSize, logoGap) + 400);
    addButton("HOME", areas[0], [this] { goTo(ScreenId::Welcome); });
    addButton(
        "DELETE", areas[1],
        [this] {
            context_.highScores.clear();
            refreshLines();
        },
        context.assets.deleteButton.get());
    addButton("QUIT", areas[2], [this] { goTo(ScreenId::Quit); });
}

void ScoresScreen::onEnter() {
    refreshLines();
}

void ScoresScreen::draw() {
    drawMenuBackground();
    drawLogo(logoSize, logoGap);

    int y = titleBottom(logoSize, logoGap) + 10;
    heading_.drawCentered(screenCenterX, y);
    y += heading_.height() + lineSpacing;

    if (lines_.empty()) {
        emptyMessage_.drawCentered(screenCenterX, y);
    }
    for (const Text& line : lines_) {
        line.drawCentered(screenCenterX, y);
        y += line.height() + lineSpacing;
    }
    drawButtons();
}

// Les lignes ne sont reconstruites que lorsque les scores changent,
// pas à chaque image.
void ScoresScreen::refreshLines() {
    const std::vector<ScoreEntry>& entries = context_.highScores.entries();
    const std::size_t count = std::min(entries.size(), config::displayedScores);

    lines_.clear();
    for (std::size_t i = 0; i < count; ++i) {
        const ScoreEntry& entry = entries[i];
        lines_.emplace_back(context_.renderer, context_.assets.smallFont.get(), config::white,
                            entry.name + " | Score : " + std::to_string(entry.score) +
                                " | Kills : " + std::to_string(entry.kills));
    }
}
