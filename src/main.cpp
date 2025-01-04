#include "game.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    // Initialiser SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        cout << "Erreur d'initialisation de SDL: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        cout << "Erreur d'initialisation des images PNG: " << SDL_GetError() << endl;
        SDL_Quit();
        return 2;
    }

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init Error: %s", TTF_GetError());
        SDL_Quit();
        return 7;
    }

    initFonts();
    // Créer une fenêtre
    SDL_Window *window = SDL_CreateWindow("Projet Poisson",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        cout << "Erreur de création de la fenêtre: " << SDL_GetError() << endl;
        SDL_Quit();
        return 3;
    }

    // Créer un renderer avec SDL_RENDERER_SOFTWARE
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        cout << "Erreur de création du renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 4;
    }

    // Ajout du background
    SDL_Surface *backgroundSurface = IMG_Load(IMGPATH::background);
    if (backgroundSurface == nullptr) {
        cout << "Erreur de chargement background:" << SDL_GetError() << endl;
        return 5;
    }

    SDL_Texture *backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
    if (backgroundTexture == nullptr) {
        cout << "Erreur de création de texture background" << SDL_GetError() << endl;
        return 6;
    }
    SDL_FreeSurface(backgroundSurface);

    // Permet d'activé l'opacité, et de créer cet assombrissement lors du timer
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);

    BoidSimulation* simulation = new BoidSimulation(renderer);
    Player* player = new Player(renderer);

    Timer* timer = new Timer(renderer);
    Uint32 lastUpdate = SDL_GetTicks();
    Scoreboard* scoreboard = new Scoreboard(renderer);

    welcomeScreen* welcome = new welcomeScreen(renderer);

    endScreen* finish = new endScreen(renderer);

    pauseScreen* pause = new pauseScreen(renderer);

    scoreScreen* score = new scoreScreen(renderer);

    SDL_Event event;
    SDL_Rect windowRect = SDL_Rect(0,0,WINDOW_WIDTH, WINDOW_HEIGHT);

    // Boucle principale
    int angle = 0;
    string tempName = "";
    bool moving = false;
    int movingReset = 0;
    bool nameAsked = false;
    bool running = true;
    int runningType = 0; // 0 = accueil / 1 = jeu / 2 = fin de la game / 3 = Menu pause / 4 = Menu score
    int iterSimu = 0;
    while (running){
        Uint32 debutFrameTimer = SDL_GetTicks();
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                // Toute cette partie va être envoyé dans une fonction, après finition de la V1 du jeu, lors de l'amélioration du code
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (runningType == 3) {
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            runningType = 1;
                        }
                    }
                    else if (runningType == 1) {
                        switch (event.key.keysym.sym) {
                            case SDLK_ESCAPE:
                                runningType = 3;
                                break;

                            case SDLK_BACKSPACE:
                                if(!tempName.empty()) {
                                    // Suppression du dernier caractère
                                    tempName.pop_back();
                                }
                                break;

                            case SDLK_RETURN:
                                // Appui sur Entrée pour terminer la saisie du pseudo
                                if(!tempName.empty()) { // On évite les pseudos vides (pas de problèmes, juste 0 intérêts)
                                    SDL_StopTextInput();
                                    player->setName(tempName);
                                    nameAsked = true; // Quitter la boucle de demande de pseudo
                                }
                                break;

                            default:
                                if(timer->getTime() <= 60) {
                                    checkDeplacement(renderer, simulation, player, event, &moving, &angle);
                                }
                                break;
                        }
                    }
                    break;

                case SDL_TEXTINPUT:
                    // Ajout du texte saisi au pseudo
                    if (tempName.size() < 16) {
                        tempName += event.text.text;
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    checkClick(welcome, event, &runningType, finish, pause, score);
                    break;

                default:
                    break;
            }
        }
        if(runningType == 0){ // Affichage de l'écran d'accueil
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            welcome->drawWelcomeScreen(renderer);
            iterSimu = 0;
        }
        else if(runningType == 1){ // Affichage du jeu*
            if(iterSimu == 0) {
                simulation->reset();
                player->reset();
                timer->reset();
                iterSimu = 1;
                moving = false;
                SDL_StartTextInput(); // Lancer la prise en compte de la saisie de texte
            }
            if(!nameAsked){
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
                player->drawAskName(renderer, tempName);
            } else {
                if(timer->getTime() > 60){ // Timer de démarage
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, backgroundTexture, &simulation->camera, &windowRect);
                    simulation->update(renderer);
                    player->drawPlayer(renderer, angle, moving);
                    scoreboard->drawPlayerKillCount(renderer, player);
                    scoreboard->drawPlayerScore(renderer, player);
                    timer->drawStartTimer(renderer);
                    // fonction de dessin du plus gros timer
                } else {
                    SDL_RenderClear(renderer);
                    SDL_RenderCopy(renderer, backgroundTexture, &simulation->camera, &windowRect);
                    simulation->update(renderer);
                    player->drawPlayer(renderer, angle, moving);
                    player->checkEveryColisions(simulation);
                    simulation->respawnBoid();
                    timer->drawTimer(renderer);
                    scoreboard->drawPlayerKillCount(renderer, player);
                    scoreboard->drawPlayerScore(renderer, player);

                    movingReset++;
                    movingReset %= 4;
                    if (movingReset == 0) {
                        moving = false;
                    }
                }

                Uint32 currentTime = SDL_GetTicks();    // Temps actuel en millisecondes
                if (currentTime > lastUpdate + 1000) {  // Si 1000 ms (1 seconde) se sont écoulées
                    timer->decrementTimer();            // Incrémenter le timer
                    lastUpdate = currentTime;           // Mettre à jour le dernier timestamp
                }

                if (timer->getTime() == 0) {
                    runningType = 2;
                    finish->enterScore(player);
                }
            }
        } else if(runningType == 2){
            iterSimu = 0;
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            finish->draw(renderer, player);

        } else if(runningType == 3){
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            pause->draw(renderer);
        } else if(runningType == 4){
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
            score->draw(renderer);
        }
        else{
            running = false;
        }

        Uint32 finFrameTimer = SDL_GetTicks() - debutFrameTimer;
        if (finFrameTimer < FRAME_DURATION) { // ce test évite d'avoir un delay négatif pour les pcs NUL
            SDL_Delay(FRAME_DURATION - finFrameTimer); // on peut fixer à la vitesse que on veut, les fps seront constants
        }
        
        // Présenter le rendu
        SDL_RenderPresent(renderer);
    }

    // Détruire le renderer et la fenêtre, et quitter SDL
    TTF_CloseFont(FONT::font1);
    TTF_CloseFont(FONT::font2);
    TTF_CloseFont(FONT::font3);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


