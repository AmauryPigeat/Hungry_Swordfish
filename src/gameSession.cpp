#include "gameSession.hpp"

#include "config.hpp"

#include <algorithm>

GameSession::GameSession(SDL_Renderer* renderer, const Assets& assets)
    : assets_(assets), player_(assets), fish_(assets), bonuses_(renderer, assets) {}

void GameSession::start(const std::string& playerName) {
    playerName_ = playerName;
    countdown_ = config::countdownDuration;
    sprintTimeLeft_ = 0.0f;
    doubleScoreTimeLeft_ = 0.0f;
    score_ = 0;
    kills_ = 0;

    player_.reset(randomPlayerStart());
    wheelZoom_ = 1.0f;
    camera_.zoom = autoZoom();
    camera_.centerOn(player_.position());
    fish_.reset(noSpawnArea());
    bonuses_.reset(noSpawnArea());
}

void GameSession::update(float dt, const PlayerInput& input) {
    // Les poissons nagent et les bulles flottent déjà pendant le décompte, mais le joueur attend.
    fish_.update(player_.position());
    bonuses_.update(dt);
    if (inCountdown()) {
        countdown_ -= dt;
        updateCamera(dt);  // la molette marche aussi pendant le décompte
        return;
    }

    float speedMultiplier = 1.0f;
    if (sprintTimeLeft_ > 0.0f) {
        speedMultiplier = config::sprintMultiplier;
    }
    player_.update(dt, input, speedMultiplier);
    updateCamera(dt);

    const EatResult eaten = fish_.eatTouching(player_.jawHitbox());
    int pointsMultiplier = 1;
    if (doubleScoreTimeLeft_ > 0.0f) {
        pointsMultiplier = config::scoreMultiplier;
    }
    kills_ += eaten.count;
    score_ += eaten.points * pointsMultiplier;
    player_.grow(eaten.count);

    for (const BonusType type : bonuses_.takeTouching(player_.hitbox())) {
        applyBonus(type);
    }

    fish_.respawnMissing(noSpawnArea());
    bonuses_.respawnMissing(noSpawnArea());

    sprintTimeLeft_ = std::max(0.0f, sprintTimeLeft_ - dt);
    doubleScoreTimeLeft_ = std::max(0.0f, doubleScoreTimeLeft_ - dt);
}

void GameSession::draw(SDL_Renderer* renderer) const {
    // Le fond couvre tout le monde : on le place comme n'importe quel objet du monde,
    // et SDL ne dessine que la partie qui tombe dans l'écran.
    // En gros dézoom (mode dev), on voit du noir autour du monde.
    const SDL_FRect world{0.0f, 0.0f, config::worldWidth, config::worldHeight};
    const SDL_FRect destination = camera_.toScreen(world);
    SDL_RenderCopyF(renderer, assets_.background.get(), nullptr, &destination);
    drawWorldBorder(renderer);

    bonuses_.draw(renderer, camera_);
    fish_.draw(renderer, camera_);
    player_.draw(renderer, camera_);
}

// Bordure rouge : 4 bandes le long des bords, à l'intérieur du monde.
void GameSession::drawWorldBorder(SDL_Renderer* renderer) const {
    const float w = config::worldWidth;
    const float h = config::worldHeight;
    const float t = config::worldBorderThickness;
    const SDL_FRect bands[4] = {
        {0.0f, 0.0f, w, t},      // haut
        {0.0f, h - t, w, t},     // bas
        {0.0f, 0.0f, t, h},      // gauche
        {w - t, 0.0f, t, h},     // droite
    };

    const SDL_Color color = config::worldBorderColor;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < 4; ++i) {
        SDL_FRect onScreen = camera_.toScreen(bands[i]);

        // En gros dézoom (mode dev), la bande deviendrait plus fine qu'un pixel et disparaîtrait :
        // on lui garde au moins 2 pixels d'épaisseur à l'écran.
        if (onScreen.w < 2.0f) {
            onScreen.w = 2.0f;
        }
        if (onScreen.h < 2.0f) {
            onScreen.h = 2.0f;
        }
        SDL_RenderFillRectF(renderer, &onScreen);
    }
}

// Position de départ au hasard, à au moins playerSpawnMargin du bord.
Vec2 GameSession::randomPlayerStart() {
    std::uniform_real_distribution<float> randomX(config::playerSpawnMargin,
                                                  config::worldWidth - config::playerSpawnMargin);
    std::uniform_real_distribution<float> randomY(config::playerSpawnMargin,
                                                  config::worldHeight - config::playerSpawnMargin);
    return {randomX(random_), randomY(random_)};
}

SDL_FRect GameSession::noSpawnArea() const {
    if (config::devMode) {
        return {0.0f, 0.0f, 0.0f, 0.0f};  // aucun point n'est dans un rectangle vide
    }

    constexpr float margin = 60.0f;
    const SDL_FRect view = camera_.view();
    const SDL_FRect area{view.x - margin, view.y - margin, view.w + 2 * margin, view.h + 2 * margin};

    // L'espadon n'a pas de taille max : très gros, le dézoom auto peut montrer toute la zone
    // où poissons et bulles apparaissent. Il n'y a alors plus aucune place hors de l'écran,
    // et la recherche d'une position ne finirait jamais : on les laisse apparaître partout.
    const float edge = config::spawnEdgeMargin;
    const bool coversWidth = area.x <= edge && area.x + area.w >= config::worldWidth - edge;
    const bool coversHeight = area.y <= edge && area.y + area.h >= config::worldHeight - edge;
    if (coversWidth && coversHeight) {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }
    return area;
}

void GameSession::zoomWithWheel(int steps) {
    // Chaque cran multiplie (ou divise) le zoom par wheelZoomStep.
    for (int i = 0; i < steps; ++i) {
        wheelZoom_ *= config::wheelZoomStep;
    }
    for (int i = 0; i > steps; --i) {
        wheelZoom_ /= config::wheelZoomStep;
    }

    if (config::devMode) {
        // Mode dev : pas de limite de jeu.
        wheelZoom_ = std::clamp(wheelZoom_, config::devMinZoom, config::devMaxZoom);
    } else {
        // On ne peut pas dézoomer plus que le dézoom auto (wheelZoom_ = 1),
        // seulement zoomer jusqu'à maxWheelZoom.
        wheelZoom_ = std::clamp(wheelZoom_, 1.0f, config::maxWheelZoom);
    }
}

float GameSession::autoZoom() const {
    // Mode dev : pas de dézoom auto, seule la molette décide du zoom.
    if (config::devMode) {
        return 1.0f;
    }

    // Exemple avec autoZoomStrength = 0.5 : l'espadon a grossi de 80 % (scale = 1.8),
    // la caméra recule de la moitié, soit 40 % : zoom = 1 / 1.4 = 0.71.
    const float growth = player_.scale() - 1.0f;
    return 1.0f / (1.0f + growth * config::autoZoomStrength);
}

void GameSession::updateCamera(float dt) {
    const float wantedZoom = autoZoom() * wheelZoom_;

    // Comme pour la vitesse du joueur : on se rapproche petit à petit du zoom voulu,
    // pour que le zoom à la molette soit fluide et pas saccadé.
    const float blend = std::min(1.0f, config::zoomSmoothing * dt);
    camera_.zoom = camera_.zoom + (wantedZoom - camera_.zoom) * blend;

    camera_.centerOn(player_.position());
}

// Les durées se cumulent : reprendre un bonus encore actif ajoute ses secondes au temps restant.
void GameSession::applyBonus(BonusType type) {
    switch (type) {
        case BonusType::Sprint:
            sprintTimeLeft_ += config::bonusEffectDuration;
            break;
        case BonusType::DoubleScore:
            doubleScoreTimeLeft_ += config::bonusEffectDuration;
            break;
    }
}
