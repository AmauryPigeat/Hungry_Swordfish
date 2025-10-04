#include "endScreen.hpp"

EndScreen::EndScreen(SDL_Renderer* renderer){
    SDL_Surface* logoImageSurface = IMG_Load(IMGPATH::logo);
    this->logoImageTexture = SDL_CreateTextureFromSurface(renderer, logoImageSurface);
    this->logoImagePos = {WINDOW_WIDTH/2 - logoImageSurface->w/8, 30, logoImageSurface->w/4, logoImageSurface->h/4};
    SDL_FreeSurface(logoImageSurface);

    SDL_Surface* logoTextSurface = TTF_RenderText_Solid(FONT::font1, "HUNGRY SWORDFISH", FONT::color);
    this->logoTextTexture = SDL_CreateTextureFromSurface(renderer, logoTextSurface);
    this->logoTextPos = {WINDOW_WIDTH/2 - logoTextSurface->w / 2, this->logoImagePos.y + this->logoImagePos.h + 30, logoTextSurface->w, logoTextSurface->h};
    SDL_FreeSurface(logoTextSurface);

    SDL_Surface* buttonBackgroundSurface = IMG_Load(IMGPATH::buttonBackground);
    this->buttonBackgroundTexture = SDL_CreateTextureFromSurface(renderer, buttonBackgroundSurface);

    SDL_Surface* homeButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "HOME", FONT::color);
    this->homeButtonTextTexture = SDL_CreateTextureFromSurface(renderer, homeButtonTextSurface);
    this->buttonHomeBackPos = {WINDOW_WIDTH/2 - buttonBackgroundSurface->w/(2*4), this->logoTextPos.y + this->logoTextPos.h + 250, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonHomeTextPos = {this->buttonHomeBackPos.x + homeButtonTextSurface->w / 2 + 18, this->buttonHomeBackPos.y + 12, homeButtonTextSurface->w, homeButtonTextSurface->h};
    SDL_FreeSurface(homeButtonTextSurface);

    SDL_Surface* reStartButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "RE-START", FONT::color);
    this->reStartButtonTextTexture = SDL_CreateTextureFromSurface(renderer, reStartButtonTextSurface);
    this->buttonReStartBackPos = {this->buttonHomeBackPos.x - buttonBackgroundSurface->w/4 - 80, this->buttonHomeBackPos.y, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonReStartTextPos = {this->buttonReStartBackPos.x + reStartButtonTextSurface->w / 6 - 9, this->buttonReStartBackPos.y + 12, reStartButtonTextSurface->w, reStartButtonTextSurface->h};
    SDL_FreeSurface(reStartButtonTextSurface);

    SDL_Surface* quitButtonTextSurface = TTF_RenderText_Solid(FONT::font1, "QUIT", FONT::color);
    this->quitButtonTextTexture = SDL_CreateTextureFromSurface(renderer, quitButtonTextSurface);
    this->buttonQuitBackPos = {this->buttonHomeBackPos.x  + buttonBackgroundSurface->w/4 + 80, this->buttonHomeBackPos.y, buttonBackgroundSurface->w/4, buttonBackgroundSurface->h/4};
    this->buttonQuitTextPos = {this->buttonQuitBackPos.x + quitButtonTextSurface->w / 2 + 18, this->buttonQuitBackPos.y + 12, quitButtonTextSurface->w, quitButtonTextSurface->h};
    SDL_FreeSurface(quitButtonTextSurface);

    SDL_FreeSurface(buttonBackgroundSurface);
}

EndScreen::~EndScreen() {
    SDL_DestroyTexture(this->logoImageTexture);
    SDL_DestroyTexture(this->logoTextTexture);
    SDL_DestroyTexture(this->buttonBackgroundTexture);
    SDL_DestroyTexture(this->reStartButtonTextTexture);
    SDL_DestroyTexture(this->homeButtonTextTexture);
    SDL_DestroyTexture(this->quitButtonTextTexture);
    SDL_DestroyTexture(this->killCountTextTexture);
    SDL_DestroyTexture(this->scoreTextTexture);
}

void EndScreen::draw(SDL_Renderer *renderer, Player *player) {
    // Affichage du logo
    SDL_RenderCopy(renderer, this->logoImageTexture, NULL, &this->logoImagePos);
    SDL_RenderCopy(renderer, this->logoTextTexture, NULL,&this->logoTextPos);

    // Affichage du score
    SDL_Color color = {255,255,255,255};
    SDL_Surface* scoreTextSurface = TTF_RenderText_Solid(FONT::font1, ("Your final score : " + std::to_string(player->getScorePoint())).c_str(), color);
    this->scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
    this->scoreTextPos = {WINDOW_WIDTH/2 - scoreTextSurface->w/2, this->logoTextPos.y + this->logoTextPos.h + 50,scoreTextSurface->w, scoreTextSurface->h};
    SDL_FreeSurface(scoreTextSurface);

    SDL_Surface* killCountTextSurface = TTF_RenderText_Solid(FONT::font1, ("Your fish kills : " + std::to_string(player->getKillCount())).c_str(), color);
    this->killCountTextTexture = SDL_CreateTextureFromSurface(renderer, killCountTextSurface);
    this->killCountTextPos = {WINDOW_WIDTH/2 - scoreTextSurface->w/2, this->scoreTextPos.y + this->scoreTextPos.h + 50, killCountTextSurface->w, killCountTextSurface->h};
    SDL_FreeSurface(killCountTextSurface);

    SDL_RenderCopy(renderer, this->scoreTextTexture, NULL, &this->scoreTextPos);
    SDL_RenderCopy(renderer, this->killCountTextTexture, NULL, &this->killCountTextPos);
    // Affichage des boutons
    // Bouton Re-Start
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonReStartBackPos);
    SDL_RenderCopy(renderer, this->reStartButtonTextTexture, NULL, &this->buttonReStartTextPos);

    // Bouton Home
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonHomeBackPos);
    SDL_RenderCopy(renderer, this->homeButtonTextTexture, NULL, &this->buttonHomeTextPos);

    // Bouton Quit
    SDL_RenderCopy(renderer, this->buttonBackgroundTexture, NULL, &this->buttonQuitBackPos);
    SDL_RenderCopy(renderer, this->quitButtonTextTexture, NULL, &this->buttonQuitTextPos);
}

void EndScreen::createScoreFile() {
    std::string nomFichier = "scoreDB.txt";

    // Création du fichier
    std::ofstream fichier(nomFichier);

    // Vérification si le fichier est créé avec succès
    if(!fichier) {
        std::cerr << "Erreur lors de la création du fichier." << std::endl;
    }
}

void EndScreen::enterScore(Player *player) {
    std::string nomFichier = "scoreDB.txt";

    if(!std::filesystem::exists(nomFichier)){
        this->createScoreFile();
    }

    // Ouverture en lecture
    std::ifstream fichierLecture(nomFichier);

    // Récupération du contenu pour écrire le nouveau score en première ligne
    std::stringstream contenu;
    contenu << fichierLecture.rdbuf();

    // Ouverture du fichier en "trunc" donc write, suppression de toutes les lignes, et écriture
    std::ofstream fichier(nomFichier, std::ios::trunc);

    // Création de la ligne dans le format personnalisé
    std::string ligne = player->getName() + ";" + std::to_string(player->getKillCount()) + ";" + std::to_string(player->getScorePoint());

    fichier << ligne << std::endl;
    fichier << contenu.str();
    fichier.close();
}
