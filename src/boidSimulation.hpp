#ifndef PROJET_POISSON_POISSON_HPP
#define PROJET_POISSON_POISSON_HPP

#include "constants.hpp"

struct Boid {
    int x, y;      // Position
    float vx, vy; // Vitesse
    SDL_Rect pos;
    /*
     * https://learn.microsoft.com/en-us/cpp/cpp/data-type-ranges?view=msvc-170
     * utilisation de type plus petit car beaucoup de boid
     * Réduction de la place utilisées en mémoire par le projet
     */
    unsigned __int8 gifIter;
    unsigned __int8 type;
    unsigned __int8 sizeH = 45;
    unsigned __int8 sizeW = 36;
    Boid(int x, int y, double vx, double vy, int type): x(x), y(y), vx(vx), vy(vy), type(type){
        pos.x = x;
        pos.y = y;
        pos.w = sizeW;
        pos.h = sizeH;
        gifIter = 0;
    }

};

class BoidSimulation {
private:
    const float visualRange = 100.0;
    const float protectedRange = 40.0;
    const float centeringFactor = 0.005;
    const float matchingFactor = 0.04;
    const float avoidFactor = 0.10;
    const float turnFactor = 0.2;
    const float minSpeed = 2.0;
    const float maxSpeed = 6.0;
    const unsigned int boidQuantity = 100;

public:
    std::vector<Boid> boids;
    SDL_Texture* fishTexture[3][5];

    SDL_Rect camera;

    BoidSimulation(SDL_Renderer *renderer);
    ~BoidSimulation();

    void addBoid(Boid boid) {
        boids.push_back(boid);
    }

    void update(SDL_Renderer *renderer);
    void drawBoidSurface(SDL_Renderer* renderer, Boid& boid);
    void respawnBoid();
    void reset();
};

#endif //PROJET_POISSON_POISSON_HPP
