#include "scoreScreen.hpp"

scoreScreen::scoreScreen(SDL_Renderer* renderer){
    SDL_Surface* logoImageSurface = IMG_Load(IMGPATH::logo);
    this->logoImageTexture = SDL_CreateTextureFromSurface(renderer, logoImageSurface);
    this->logoImagePos = {WINDOW_WIDTH/2 - logoImageSurface->w/12, 30, logoImageSurface->w/6, logoImageSurface->h/6};
    SDL_FreeSurface(logoImageSurface);

    SDL_Surface* logoTextSurface = TTF_RenderText_Solid(FONT::font1, "HUNGRY SWORDFISH", FONT::color);
    this->logoTextTexture = SDL_CreateTextureFromSurface(renderer, logoTextSurface);
    this->logoTextPos = {WINDOW_WIDTH/2 - logoTextSurface->w/2, this->logoImagePos.y + this->logoImagePos.h + 10, logoTextSurface->w, logoTextSurface->h};
    SDL_FreeSurface(logoTextSurface);

    SDL_Surface* buttonBackgroundSurface = IMG_Load(IMGPATH::buttonBackground);
    this->buttonBackgroundTexture = SDL_CreateTextureFromSurface(renderer, buttonBackgroundSurface);

    SDL_Surface* deleteAllButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "DELETE", FONT::color);
    SDL_Surface* deleteAllButtonBackSurface = IMG_Load(IMGPATH::buttonDeleteBackground);
    this->deleteAllButtonTextTexture = SDL_CreateTextureFromSurface(renderer, deleteAllButtonTextSurface);
    this->deleteAllButtonBackTexture = SDL_CreateTextureFromSurface(renderer, deleteAllButtonBackSurface);
    this->buttonDeleteBackPos = {WINDOW_WIDTH/2 - buttonBackgroundSurface->w/(2*4), this->logoTextPos.y + this->logoTextPos.h + 400, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonDeleteTextPos = {this->buttonDeleteBackPos.x + 40, this->buttonDeleteBackPos.y + 12, deleteAllButtonTextSurface->w, deleteAllButtonTextSurface->h};
    SDL_FreeSurface(deleteAllButtonTextSurface);
    SDL_FreeSurface(deleteAllButtonBackSurface);

    SDL_Surface* homeButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "HOME", FONT::color);
    this->homeButtonTextTexture = SDL_CreateTextureFromSurface(renderer, homeButtonTextSurface);
    this->buttonHomeBackPos = {this->buttonDeleteBackPos.x - buttonBackgroundSurface->w/4 - 80, this->buttonDeleteBackPos.y, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonHomeTextPos = {this->buttonHomeBackPos.x + homeButtonTextSurface->w / 2 + 18, this->buttonHomeBackPos.y + 12, homeButtonTextSurface->w, homeButtonTextSurface->h};
    SDL_FreeSurface(homeButtonTextSurface);

    SDL_Surface* quitButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "QUIT", FONT::color);
    this->quitButtonTextTexture = SDL_CreateTextureFromSurface(renderer, quitButtonTextSurface);
    this->buttonQuitBackPos = {this->buttonDeleteBackPos.x  + buttonBackgroundSurface->w/4 + 80, this->buttonHomeBackPos.y, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonQuitTextPos = {this->buttonQuitBackPos.x + quitButtonTextSurface->w / 2 + 18, this->buttonQuitBackPos.y + 12, quitButtonTextSurface->w, quitButtonTextSurface->h};
    SDL_FreeSurface(quitButtonTextSurface);
    SDL_FreeSurface(buttonBackgroundSurface);

    SDL_Surface* mainTextSurface = TTF_RenderText_Solid(FONT::font1, "Last 10 scores", FONT::color);
    this->mainTextTexture = SDL_CreateTextureFromSurface(renderer, mainTextSurface);
    this->mainTextPos = {WINDOW_WIDTH/2 - mainTextSurface->w/2, this->logoTextPos.y + mainTextSurface->h + 10, mainTextSurface->w, mainTextSurface->h};
    SDL_FreeSurface(mainTextSurface);

}

scoreScreen::~scoreScreen() {
    SDL_DestroyTexture(this->logoImageTexture);
    SDL_DestroyTexture(this->logoTextTexture);
    SDL_DestroyTexture(this->buttonBackgroundTexture);
    SDL_DestroyTexture(this->homeButtonTextTexture);
    SDL_DestroyTexture(this->quitButtonTextTexture);
    SDL_DestroyTexture(this->deleteAllButtonBackTexture);
    SDL_DestroyTexture(this->deleteAllButtonTextTexture);
    SDL_DestroyTexture(this->mainTextTexture);
}

Score* scoreScreen::getEveryScore() {
    Score* scoresList = new Score[10];
    std::string nomFichier = "scoreDB.txt";
    std::ifstream fichier(nomFichier, std::ios::in);
    std::string ligne;
    __int8 iter = 0;
    Score empty = {"NULL", -1, -1}; // il n'est pas possible en jouant normalement d'avoir -1 de score ou killcount

    while (std::getline(fichier, ligne)) {
        scoresList[iter] = parseLineToScore(ligne);
        iter++;
    }
    for(__int8 i = iter;i < 10;i++){
        scoresList[i] = empty;
    }
    return scoresList;
}

void scoreScreen::draw(SDL_Renderer *renderer) {
    // Affichage du logo
    SDL_RenderCopy(renderer, this->logoImageTexture, NULL, &this->logoImagePos);
    SDL_RenderCopy(renderer, this->logoTextTexture, NULL,&this->logoTextPos);

    // Affichage du text principal
    SDL_RenderCopy(renderer, this->mainTextTexture, NULL, &this->mainTextPos);

    // Affichage des boutons
    // Bouton Delete
    SDL_RenderCopy(renderer, this->deleteAllButtonBackTexture, NULL, &this->buttonDeleteBackPos);
    SDL_RenderCopy(renderer, this->deleteAllButtonTextTexture, NULL, &this->buttonDeleteTextPos);

    // Bouton Home
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonHomeBackPos);
    SDL_RenderCopy(renderer, this->homeButtonTextTexture, NULL, &this->buttonHomeTextPos);

    // Bouton Quit
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonQuitBackPos);
    SDL_RenderCopy(renderer, this->quitButtonTextTexture, NULL, &this->buttonQuitTextPos);

    // Affichage des scores
    Score* scoresList = this->getEveryScore();
    __int8 iter = 0;
    Score empty  = {"NULL", -1, -1};
    std::string ligne;
    SDL_Surface* ligneSurface;
    SDL_Texture* ligneTexture;
    SDL_Rect textPos = {0, this->mainTextPos.y + 10, 0, 0};
    SDL_Color color = {255,255,255,255};

    while(iter < 10) {
        if(scoresList[iter].scorePoint > empty.scorePoint && scoresList[iter].killCount > empty.killCount) {
            ligne = "Player name : " + scoresList[iter].name + " | Score : " + std::to_string(scoresList[iter].scorePoint)
                      + " | Kill count : " + std::to_string(scoresList[iter].killCount);
            ligneSurface = TTF_RenderText_Solid(FONT::font2, ligne.c_str(), FONT::color);
            textPos = {WINDOW_WIDTH/2 - ligneSurface->w/2, textPos.y + ligneSurface->h + 8, ligneSurface->w, ligneSurface->h};
            ligneTexture = SDL_CreateTextureFromSurface(renderer, ligneSurface);
            SDL_RenderCopy(renderer, ligneTexture, NULL, &textPos);
            SDL_FreeSurface(ligneSurface);
            SDL_DestroyTexture(ligneTexture);
        } else{
            iter = 10;
        }
        iter++;
    }

}

void scoreScreen::deleteScore() {
    // On supprime et on recréer le fichier
    const char* nomFichier = "scoreDB.txt"; // char* et non std::string car on utilise une fonction de C après

    std::ifstream fichier(nomFichier);
    if(fichier){
        std::remove(nomFichier);
    }
    fichier.close();

    std::ofstream fichierNew(nomFichier);   
}

Score parseLineToScore(const std::string& ligne) {
    Score score;

    std::istringstream flux(ligne);
    std::string element;

    // Extraire les éléments séparés par ";"
    if (std::getline(flux, score.name, ';') &&
        std::getline(flux, element, ';')) {
        // Convertir le 2ème élément en entier pour killCount
        score.killCount = std::stoi(element);

        if (std::getline(flux, element, ';')) {
            // Convertir le 3ème élément en entier pour scorePoint
            score.scorePoint = std::stoi(element);
        }
    }

    return score;
}
