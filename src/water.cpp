#include "water.hpp"

#include "config.hpp"

#include <algorithm>

namespace {

// Numéro de l'image à afficher maintenant. Toutes les tuiles montrent la même image :
// l'eau ondule partout en même temps. On se base sur l'horloge de SDL, donc l'eau
// bouge aussi pendant la pause.
std::size_t currentFrame(const Animation& water) {
    const float seconds = static_cast<float>(SDL_GetTicks()) / 1000.0f;
    const auto frameNumber = static_cast<std::size_t>(seconds / config::waterFrameDuration);
    return frameNumber % water.size();
}

}  // namespace

void drawWater(SDL_Renderer* renderer, const Animation& water, const Camera& camera, float width,
               float height) {
    SDL_Texture* frame = water[currentFrame(water)].get();
    const float tile = config::waterTileSize;

    // Numéros de la dernière colonne et de la dernière ligne de tuiles du monde.
    const int lastColumnInWorld = static_cast<int>((width - 1.0f) / tile);
    const int lastRowInWorld = static_cast<int>((height - 1.0f) / tile);

    // Tuiles visibles : celles qui touchent la vue de la caméra, sans sortir du monde.
    const SDL_FRect view = camera.view();
    const int firstColumn = std::max(0, static_cast<int>(view.x / tile));
    const int firstRow = std::max(0, static_cast<int>(view.y / tile));
    const int lastColumn = std::min(lastColumnInWorld, static_cast<int>((view.x + view.w) / tile));
    const int lastRow = std::min(lastRowInWorld, static_cast<int>((view.y + view.h) / tile));

    for (int row = firstRow; row <= lastRow; ++row) {
        for (int column = firstColumn; column <= lastColumn; ++column) {
            // Position de la tuile dans le monde. La dernière colonne / ligne peut être
            // coupée : on ne prend alors que le début de l'image.
            const float left = static_cast<float>(column) * tile;
            const float top = static_cast<float>(row) * tile;
            const float tileWidth = std::min(tile, width - left);
            const float tileHeight = std::min(tile, height - top);

            // L'image (waterImageSize) est réduite à la taille de la tuile (waterTileSize) :
            // pour une tuile coupée, on prend la même proportion de l'image.
            const float imagePixelsPerTilePixel = config::waterImageSize / tile;
            const SDL_Rect source{0, 0, static_cast<int>(tileWidth * imagePixelsPerTilePixel),
                                  static_cast<int>(tileHeight * imagePixelsPerTilePixel)};

            // On convertit les deux coins séparément : deux tuiles voisines calculent ainsi
            // exactement le même bord, sans fente d'un pixel entre elles quand on zoome.
            const Vec2 topLeft = camera.toScreen(Vec2{left, top});
            const Vec2 bottomRight = camera.toScreen(Vec2{left + tileWidth, top + tileHeight});
            const SDL_FRect destination{topLeft.x, topLeft.y, bottomRight.x - topLeft.x,
                                        bottomRight.y - topLeft.y};

            SDL_RenderCopyF(renderer, frame, &source, &destination);
        }
    }
}
