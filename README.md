# Hungry Swordfish

Petit jeu 2D en C++ / SDL2 : un espadon affamé a 60 secondes pour manger le plus de poissons possible.
Les poissons nagent en bancs (algorithme des *boids*) et fuient à l'approche du joueur.

## Commandes

| Touche | Action |
| --- | --- |
| Flèches ou ZQSD | Nager (diagonales possibles) |
| Échap | Pause / reprendre |
| F11 | Plein écran |

## Compilation

Dépendances : CMake ≥ 3.25, un compilateur C++20, SDL2, SDL2_image, SDL2_ttf.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/hungry_swordfish
```

Sous Windows, SDL2 est cherché dans `C:/SDL2`. Pour un autre dossier :
`cmake -S . -B build -DCMAKE_PREFIX_PATH=D:/chemin/vers/SDL2`.

Le jeu cherche les dossiers `img/` et `fonts/` à côté de l'exécutable ou dans le dossier parent,
il peut donc être lancé tel quel depuis `build/` ou `cmake-build-debug/`.
Les scores sont enregistrés dans `scoreDB.txt`, à côté de l'exécutable.

## Organisation du code

| Fichier | Rôle |
| --- | --- |
| `main.cpp` | Lance le jeu, affiche les erreurs de démarrage |
| `game` | Fenêtre, boucle principale à pas fixe, changement d'écran |
| `gameSession` | Règles d'une partie : chrono, score, joueur, poissons, caméra |
| `player` | Déplacement et affichage de l'espadon |
| `boidSimulation` | Bancs de poissons (boids + grille spatiale) |
| `screens/` | Un fichier par écran, `screen` contient ce qu'ils partagent |
| `assets` | Chargement des images et polices |
| `text`, `button` | Briques d'interface |
| `highScores` | Lecture / écriture du fichier de scores |
| `sdlUtils` | Initialisation SDL et libération automatique des ressources |
| `config.hpp` | Tous les réglages (tailles, vitesses, durées, couleurs) |
