#include "boidSimulation.hpp"

void BoidSimulation::update(SDL_Renderer *renderer) {
    double visualRangeSquared = visualRange * visualRange;
    double protectedRangeSquared = protectedRange * protectedRange;

    for (Boid &boid: boids) {
        double xposAvg = 0, yposAvg = 0, xvelAvg = 0, yvelAvg = 0;
        double closeDx = 0, closeDy = 0;
        int neighboringBoids = 0;

        for (const Boid &other: boids) {
            if (&boid == &other) continue;

            double dx = boid.x - other.x;
            double dy = boid.y - other.y;

            if (std::abs(dx) < visualRange && std::abs(dy) < visualRange) {
                double squaredDistance = dx * dx + dy * dy;

                if (squaredDistance < protectedRangeSquared) {
                    closeDx += dx;
                    closeDy += dy;
                } else if (squaredDistance < visualRangeSquared) {
                    xposAvg += other.x;
                    yposAvg += other.y;
                    xvelAvg += other.vx;
                    yvelAvg += other.vy;
                    neighboringBoids++;
                }
            }
        }

        if (neighboringBoids > 0) {
            xposAvg /= neighboringBoids;
            yposAvg /= neighboringBoids;
            xvelAvg /= neighboringBoids;
            yvelAvg /= neighboringBoids;

            boid.vx += (xposAvg - boid.x) * centeringFactor + (xvelAvg - boid.vx) * matchingFactor;
            boid.vy += (yposAvg - boid.y) * centeringFactor + (yvelAvg - boid.vy) * matchingFactor;
        }

        boid.vx += closeDx * avoidFactor;
        boid.vy += closeDy * avoidFactor;

        if (boid.y > ENV_HEIGHT) boid.vy -= turnFactor;
        if (boid.y < 0) boid.vy += turnFactor;
        if (boid.x > ENV_WIDTH) boid.vx -= turnFactor;
        if (boid.x < 0) boid.vx += turnFactor;

        double speed = std::sqrt(boid.vx * boid.vx + boid.vy * boid.vy);
        if (speed < minSpeed) {
            boid.vx = (boid.vx / speed) * minSpeed;
            boid.vy = (boid.vy / speed) * minSpeed;
        }
        if (speed > maxSpeed) {
            boid.vx = (boid.vx / speed) * maxSpeed;
            boid.vy = (boid.vy / speed) * maxSpeed;
        }

        boid.x += boid.vx;
        boid.y += boid.vy;
        boid.pos.x = boid.x;
        boid.pos.y = boid.y;
        drawBoidSurface(renderer, boid);
    }
}

BoidSimulation::BoidSimulation(SDL_Renderer *renderer){
    // Sétup de la caméra
    this->camera = {0, 0, CAMERA_WIDTH, CAMERA_HEIGHT};

    //initialisation du random (spawn initial des poissons)
    std::random_device rd;
    std::mt19937 gen(rd()); // Fonction de Mersenne

    // Création des limites ou les entiers
    std::uniform_int_distribution<int> randomX(0, ENV_WIDTH);
    std::uniform_int_distribution<int> randomY(1, ENV_HEIGHT);
    std::uniform_int_distribution<int> type(0, 2);
    // Création des limites pour les float
    std::uniform_real_distribution<float> randomVx(-3.0, 3.0);
    std::uniform_real_distribution<float> randomVy(-3.0, 3.0);

    // création initiale des poissons
    for(unsigned int i = 0;i < boidQuantity;i++){
        this->addBoid(Boid(randomX(gen), randomY(gen), randomVx(gen), randomVy(gen), type(gen)));
    }
    // Création des surfaces pour les images de poissons
    SDL_Surface *fishSurface;
    for(unsigned int i = 0;i < 3;i++) {
        for(unsigned int j = 0;j < 5;j++) {
            if(strcmp(IMGPATH::fishWalk[i][j], "") != 0){
                fishSurface = IMG_Load(IMGPATH::fishWalk[i][j]);
                if (fishSurface == NULL) {
                    std::cout << "Erreur de création de surface poisson " << SDL_GetError() << std::endl;
                    std::cout << IMGPATH::fishWalk[i][j] << std::endl;
                }
                fishTexture[i][j] = SDL_CreateTextureFromSurface(renderer, fishSurface);
                if (fishTexture[i][j] == NULL) {
                    std::cout << "Erreur de création de texture fishwalk " << SDL_GetError() << std::endl;
                }
                SDL_FreeSurface(fishSurface);
            } else{
                fishTexture[i][j] = NULL;
            }
        }
    }
}

BoidSimulation::~BoidSimulation(){
    for(auto & j : this->fishTexture) {
        for (SDL_Texture *i: j) {
            SDL_DestroyTexture(i);
        }
    }
}

void BoidSimulation::drawBoidSurface(SDL_Renderer* renderer, Boid& boid){
    if(boid.pos.x > (this->camera.x - 20) && boid.pos.x < (this->camera.x + WINDOW_WIDTH + 20)){
        // séparation en 2 if (x et y) pour plus de lisibilité
        if(boid.y > (this->camera.y - 20) && boid.y < (this->camera.y + WINDOW_HEIGHT + 20)){
            boid.gifIter++;
            double angle = std::atan2(boid.vy, boid.vx) * (180.0 / M_PI); // angle de rotation en dégré (sens des aiguilles)
            SDL_RendererFlip flip;
            SDL_Rect position = SDL_Rect(boid.pos.x - this->camera.x, boid.pos.y - this->camera.y, boid.sizeH, boid.sizeW);
            if(angle > 90.0 || angle < -90.0){
                flip = SDL_FLIP_VERTICAL;
            }
            else{
                flip = SDL_FLIP_NONE;
            }
            if(boid.type == 0){
                boid.gifIter %= 18;
                SDL_RenderCopyEx(renderer, this->fishTexture[boid.type][boid.gifIter / 6],  NULL, &position, angle, NULL, flip);
            } else {
                boid.gifIter %= 20;
                SDL_RenderCopyEx(renderer, this->fishTexture[boid.type][boid.gifIter / 5],  NULL, &position, angle, NULL, flip);
            }
        }
    }
}

void BoidSimulation::respawnBoid(){
    //initialisation du random (spawn initial des poissons)
    std::random_device rd;
    std::mt19937 gen(rd()); // Fonction de Mersenne

    // Création des limites ou les entiers
    std::uniform_int_distribution<int> randomX(0, ENV_WIDTH);
    std::uniform_int_distribution<int> randomY(1, ENV_HEIGHT);
    std::uniform_int_distribution<int> type(0, 2);
    // Création des limites pour les float
    std::uniform_real_distribution<float> randomVx(-3.0, 3.0);
    std::uniform_real_distribution<float> randomVy(-3.0, 3.0);

    // Nombre de poisson à faire respawn
    int missingBoid = this->boidQuantity - this->boids.size();

    for(int i = 0;i < missingBoid;i++){
        this->addBoid(Boid(randomX(gen), randomY(gen), randomVx(gen), randomVy(gen), type(gen)));
    }
}

void BoidSimulation::reset() {
    this->camera.x = 0;
    this->camera.y = 0;
    respawnBoid();
}
