#include "screens.hpp"

#include <algorithm>

namespace {

constexpr int logoSize = 162;
constexpr int logoGap = 10;
constexpr int lineSpacing = 6;

// Colonnes du tableau, en décalage par rapport au centre de l'écran.
// Rang et nom sont alignés à gauche, les nombres à droite pour que les chiffres tombent juste.
constexpr int rankLeft = -250;
constexpr int nameLeft = -200;
constexpr int scoreRight = 140;
constexpr int killsRight = 250;
constexpr int tableLeft = rankLeft - 10;
constexpr int tableRight = killsRight + 10;

SDL_Color rankColor(std::size_t rank) {
    switch (rank) {
        case 0: return config::gold;
        case 1: return config::silver;
        case 2: return config::bronze;
        default: return config::white;
    }
}

// Meilleur score d'abord, puis le plus de poissons mangés.
// Le tri stable garde la partie la plus récente devant en cas d'égalité parfaite.
std::vector<ScoreEntry> ranking(std::vector<ScoreEntry> entries) {
    std::stable_sort(entries.begin(), entries.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        if (a.score != b.score) {
            return a.score > b.score;
        }
        return a.kills > b.kills;
    });
    return entries;
}

}  // namespace

ScoresScreen::ScoresScreen(GameContext& context)
    : Screen(context),
      heading_(context.renderer, context.assets.mediumFont.get(), config::white,
               "Top " + std::to_string(config::displayedScores) + " scores"),
      emptyMessage_(context.renderer, context.assets.smallFont.get(), config::mint,
                    "No game played yet"),
      header_{Text(context.renderer, context.assets.smallFont.get(), config::mint, "#"),
              Text(context.renderer, context.assets.smallFont.get(), config::mint, "PLAYER"),
              Text(context.renderer, context.assets.smallFont.get(), config::mint, "SCORE"),
              Text(context.renderer, context.assets.smallFont.get(), config::mint, "KILLS")} {
    const std::vector<SDL_Rect> areas = buttonRow(3, titleBottom(logoSize, logoGap) + 400);
    addButton("HOME", areas[0], [this] { goTo(ScreenId::Welcome); });
    addButton(
        "DELETE", areas[1],
        [this] {
            context_.highScores.clear();
            refreshRows();
        },
        context.assets.deleteButton.get());
    addButton("QUIT", areas[2], [this] { goTo(ScreenId::Quit); });
}

void ScoresScreen::onEnter() {
    refreshRows();
}

void ScoresScreen::draw() {
    drawMenuBackground();
    drawLogo(logoSize, logoGap);

    int y = titleBottom(logoSize, logoGap) + 10;
    heading_.drawCentered(screenCenterX, y);
    y += heading_.height() + lineSpacing;

    if (rows_.empty()) {
        emptyMessage_.drawCentered(screenCenterX, y);
        drawButtons();
        return;
    }

    drawRow(header_, y);
    y += header_.name.height() + 2;

    // Filet sous l'en-tête
    const SDL_Color& line = config::mint;
    SDL_SetRenderDrawColor(context_.renderer, line.r, line.g, line.b, line.a);
    SDL_RenderDrawLine(context_.renderer, screenCenterX + tableLeft, y,
                       screenCenterX + tableRight, y);
    y += lineSpacing;

    for (const Row& row : rows_) {
        drawRow(row, y);
        y += row.name.height() + lineSpacing;
    }
    drawButtons();
}

void ScoresScreen::drawRow(const Row& row, int y) const {
    row.rank.draw(screenCenterX + rankLeft, y);
    row.name.draw(screenCenterX + nameLeft, y);
    row.score.drawRightAligned(screenCenterX + scoreRight, y);
    row.kills.drawRightAligned(screenCenterX + killsRight, y);
}

// Le tableau n'est reconstruit que lorsque les scores changent, pas à chaque image.
void ScoresScreen::refreshRows() {
    const std::vector<ScoreEntry> entries = ranking(context_.highScores.entries());
    const std::size_t count = std::min(entries.size(), config::displayedScores);

    SDL_Renderer* renderer = context_.renderer;
    TTF_Font* font = context_.assets.smallFont.get();

    rows_.clear();
    for (std::size_t i = 0; i < count; ++i) {
        const ScoreEntry& entry = entries[i];
        const SDL_Color color = rankColor(i);
        rows_.push_back(Row{Text(renderer, font, color, std::to_string(i + 1)),
                            Text(renderer, font, color, entry.name),
                            Text(renderer, font, color, std::to_string(entry.score)),
                            Text(renderer, font, color, std::to_string(entry.kills))});
    }
}
