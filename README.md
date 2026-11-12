# Hungry Swordfish

Petit jeu 2D en C++ / SDL2 : un espadon affamé a 60 secondes pour manger le plus de poissons possible.
Les poissons nagent en bancs (algorithme des *boids*) et fuient à l'approche du joueur.
L'espadon grossit un peu à chaque poisson mangé, et peut foncer d'un coup (dash) pour surprendre un banc.

Des bulles bonus flottent dans le monde :

| Bulle | Effet |
| --- | --- |
| `+5` | 5 secondes de plus au chrono |
| `>>` | Sprint : vitesse x1,5 pendant 5 s |
| `x2` | Points doublés pendant 5 s |

Le sprint et le x2 se cumulent : reprendre une bulle encore active ajoute 5 s au temps restant.

## Commandes

| Touche | Action |
| --- | --- |
| Flèches ou ZQSD | Nager (diagonales possibles) |
| Espace | Dash (recharge de 2 s, jauge en bas à gauche) |
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
| `gameSession` | Règles d'une partie : chrono, score, effets des bonus, joueur, poissons, caméra |
| `player` | Déplacement, dash, croissance et affichage de l'espadon |
| `boidSimulation` | Bancs de poissons (boids + grille spatiale) |
| `bonuses` | Bulles bonus : apparition, ramassage, affichage |
| `screens/` | Un fichier par écran, `screen` contient ce qu'ils partagent |
| `assets` | Chargement des images et polices |
| `text`, `button` | Briques d'interface |
| `highScores` | Lecture / écriture du fichier de scores |
| `sdlUtils` | Initialisation SDL et libération automatique des ressources |
| `config.hpp` | Tous les réglages (tailles, vitesses, durées, couleurs) |
