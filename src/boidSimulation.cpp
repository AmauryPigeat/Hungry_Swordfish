#include "boidSimulation.hpp"


#include <algorithm>
#include <cmath>

BoidSimulation::BoidSimulation(const Assets& assets) : assets_(assets) {
    boids_.reserve(config::fishCount);
}

void BoidSimulation::reset(const SDL_FRect& hiddenArea) {
    boids_.clear();
    respawnMissing(hiddenArea);
}

void BoidSimulation::update(Vec2 predator) {
    rebuildGrid();

    for (Boid& boid : boids_) {
        steer(boid, predator);
        boid.x += boid.vx;
        boid.y += boid.vy;

        const auto frameCount = static_cast<int>(assets_.fish[boid.species].size());
        boid.animationTick =
            static_cast<std::uint8_t>((boid.animationTick + 1) % (frameCount * ticksPerFrame));
    }
}

EatResult BoidSimulation::eatTouching(const Quad& jaw) {
    EatResult result;
    std::erase_if(boids_, [&](const Boid& boid) {
        const Species& species = speciesTable[boid.species];
        const SDL_FRect fish = centeredRect({boid.x, boid.y}, species.width, species.height);
        if (!quadsOverlap(jaw, quadFromRect(fish))) {
            return false;
        }
        result.count += 1;
        result.points += species.points;
        return true;
    });
    return result;
}

void BoidSimulation::respawnMissing(const SDL_FRect& hiddenArea) {
    while (boids_.size() < config::fishCount) {
        boids_.push_back(randomBoid(hiddenArea));
    }
}

void BoidSimulation::draw(SDL_Renderer* renderer, const Camera& camera) const {
    const SDL_FRect screen{0.0f, 0.0f, static_cast<float>(config::windowWidth),
                           static_cast<float>(config::windowHeight)};

    for (const Boid& boid : boids_) {
        const Species& species = speciesTable[boid.species];
        const Vec2 onScreen{boid.x - camera.x, boid.y - camera.y};
        const SDL_FRect destination = centeredRect(onScreen, species.width, species.height);
        if (!overlaps(destination, screen)) {
            continue;  // hors de l'écran : inutile de le dessiner
        }

        // Même principe que pour le joueur : l'image regarde vers la droite.
        const float angle = angleInDegrees({boid.vx, boid.vy});
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if (std::abs(angle) > 90.0f) {
            flip = SDL_FLIP_VERTICAL;
        }
        const Animation& animation = assets_.fish[boid.species];
        SDL_Texture* frame = animation[boid.animationTick / ticksPerFrame].get();
        SDL_RenderCopyExF(renderer, frame, nullptr, &destination, angle, nullptr, flip);
    }
}

int BoidSimulation::columnOf(float x) {
    return std::clamp(static_cast<int>(x / cellSize), 0, gridColumns - 1);
}

int BoidSimulation::rowOf(float y) {
    return std::clamp(static_cast<int>(y / cellSize), 0, gridRows - 1);
}

// Range les indices des poissons case par case (tri par comptage).
void BoidSimulation::rebuildGrid() {
    const int cellCount = gridColumns * gridRows;
    const auto cellOf = [](const Boid& boid) {
        return rowOf(boid.y) * gridColumns + columnOf(boid.x);
    };

    // 1. Nombre de poissons par case, puis position de départ de chaque case.
    cellStart_.assign(cellCount + 1, 0);
    for (const Boid& boid : boids_) {
        cellStart_[cellOf(boid) + 1] += 1;
    }
    for (int cell = 0; cell < cellCount; ++cell) {
        cellStart_[cell + 1] += cellStart_[cell];
    }

    // 2. Chaque poisson est placé dans la tranche de sa case.
    cellCursor_ = cellStart_;
    boidsByCell_.resize(boids_.size());
    for (std::size_t index = 0; index < boids_.size(); ++index) {
        const int cell = cellOf(boids_[index]);
        boidsByCell_[cellCursor_[cell]] = static_cast<int>(index);
        cellCursor_[cell] += 1;
    }
}

// Ajuste la vitesse d'un poisson en fonction de ses voisins, du joueur et des bords du monde.
void BoidSimulation::steer(Boid& boid, Vec2 predator) const {
    float sumX = 0.0f, sumY = 0.0f;    // positions des voisins visibles
    float sumVx = 0.0f, sumVy = 0.0f;  // vitesses des voisins visibles
    float awayX = 0.0f, awayY = 0.0f;  // direction pour s'écarter des voisins trop proches
    int neighbours = 0;

    const int column = columnOf(boid.x);
    const int row = rowOf(boid.y);
    for (int r = std::max(row - 1, 0); r <= std::min(row + 1, gridRows - 1); ++r) {
        for (int c = std::max(column - 1, 0); c <= std::min(column + 1, gridColumns - 1); ++c) {
            const int cell = r * gridColumns + c;
            for (int i = cellStart_[cell]; i < cellStart_[cell + 1]; ++i) {
                const Boid& other = boids_[boidsByCell_[i]];
                if (&other == &boid) {
                    continue;
                }
                const float dx = boid.x - other.x;
                const float dy = boid.y - other.y;
                const float squaredDistance = dx * dx + dy * dy;

                if (squaredDistance < protectedRange * protectedRange) {
                    awayX += dx;
                    awayY += dy;
                } else if (squaredDistance < visualRange * visualRange) {
                    sumX += other.x;
                    sumY += other.y;
                    sumVx += other.vx;
                    sumVy += other.vy;
                    neighbours += 1;
                }
            }
        }
    }

    // Cohésion (aller vers le centre du groupe) et alignement (prendre sa vitesse moyenne).
    if (neighbours > 0) {
        const float count = static_cast<float>(neighbours);
        boid.vx += (sumX / count - boid.x) * centeringFactor + (sumVx / count - boid.vx) * matchingFactor;
        boid.vy += (sumY / count - boid.y) * centeringFactor + (sumVy / count - boid.vy) * matchingFactor;
    }

    // Séparation.
    boid.vx += awayX * avoidFactor;
    boid.vy += awayY * avoidFactor;

    // Fuite : on s'éloigne du joueur s'il est assez près pour être vu.
    const Vec2 fromPredator{boid.x - predator.x, boid.y - predator.y};
    const float predatorDistance = length(fromPredator);
    if (predatorDistance < fleeRange && predatorDistance > 0.0f) {
        boid.vx += fromPredator.x / predatorDistance * fleeFactor;
        boid.vy += fromPredator.y / predatorDistance * fleeFactor;
    }

    // Demi-tour progressif à l'approche des bords.
    if (boid.x < edgeMargin) boid.vx += turnFactor;
    if (boid.x > config::worldWidth - edgeMargin) boid.vx -= turnFactor;
    if (boid.y < edgeMargin) boid.vy += turnFactor;
    if (boid.y > config::worldHeight - edgeMargin) boid.vy -= turnFactor;

    // La vitesse reste entre minSpeed et maxSpeed.
    const float speed = std::sqrt(boid.vx * boid.vx + boid.vy * boid.vy);
    if (speed < 0.0001f) {
        boid.vx = minSpeed;  // poisson à l'arrêt complet : on le relance
        boid.vy = 0.0f;
    } else {
        const float wantedSpeed = std::clamp(speed, minSpeed, maxSpeed);
        boid.vx *= wantedSpeed / speed;
        boid.vy *= wantedSpeed / speed;
    }
}

Boid BoidSimulation::randomBoid(const SDL_FRect& hiddenArea) {
    std::uniform_real_distribution<float> randomX(edgeMargin, config::worldWidth - edgeMargin);
    std::uniform_real_distribution<float> randomY(edgeMargin, config::worldHeight - edgeMargin);
    std::uniform_real_distribution<float> randomVelocity(-3.0f, 3.0f);
    std::uniform_int_distribution<int> randomSpecies(0, static_cast<int>(speciesTable.size()) - 1);

    Boid boid{};
    do {
        boid.x = randomX(random_);
        boid.y = randomY(random_);
    } while (contains(hiddenArea, {boid.x, boid.y}));

    boid.vx = randomVelocity(random_);
    boid.vy = randomVelocity(random_);
    boid.species = static_cast<std::uint8_t>(randomSpecies(random_));

    // Départ à un moment aléatoire de l'animation : les poissons ne battent pas tous en rythme.
    const auto frameCount = static_cast<int>(assets_.fish[boid.species].size());
    std::uniform_int_distribution<int> randomTick(0, frameCount * ticksPerFrame - 1);
    boid.animationTick = static_cast<std::uint8_t>(randomTick(random_));
    return boid;
}
