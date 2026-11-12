# Hungry Swordfish : fonctionnement du code

Ce document décrit chaque classe du jeu : son rôle, ses fonctionnalités principales et la façon dont elles marchent.
Pour compiler et jouer, voir le [README](README.md).

## Vue d'ensemble

```
main()
 └─ Game                      fenêtre, boucle principale, changement d'écran
     ├─ SdlContext            initialise / ferme SDL
     ├─ Assets                images et polices, chargées une seule fois
     ├─ GameSession           règles d'une partie
     │   ├─ Player            l'espadon
     │   ├─ BoidSimulation    les bancs de poissons
     │   ├─ BonusField        les bulles bonus
     │   └─ Camera            la partie du monde visible
     ├─ HighScores            fichier des scores
     └─ Screen (x6)           un objet par écran : accueil, pseudo, partie, pause, fin, scores
         └─ Button, Text      briques d'interface
```

Le principe : **`GameSession` contient les règles du jeu** et ne connaît ni le clavier ni les menus.
**Les écrans (`Screen`) gèrent l'interface** : ils lisent le clavier, appellent `GameSession` et dessinent l'interface (HUD, menus).
Tous les réglages (vitesses, durées, tailles, couleurs) sont dans `config.hpp`.

Deux systèmes de coordonnées sont utilisés :

- **le monde** : 3840 × 2160 px, où vivent le joueur, les poissons et les bulles ;
- **l'écran** : toujours 1280 × 720 px « logiques ». SDL met à l'échelle si la fenêtre est plus grande ou en plein écran.

---

## `main.cpp`

Crée le `Game` et le lance. Toute exception levée au démarrage (image manquante, SDL qui ne s'initialise pas…) est attrapée ici et affichée dans une boîte de dialogue, au lieu d'un crash silencieux.

```cpp
try {
    Game game;
    game.run();
} catch (const std::exception& error) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Hungry Swordfish", error.what(), nullptr);
    return 1;
}
```

---

## `Game` (`game.hpp/.cpp`)

Le programme lui-même : la fenêtre, le renderer, les ressources, les six écrans et la boucle principale.

### Ordre des membres

Les membres sont construits dans l'ordre de déclaration et **détruits dans l'ordre inverse**. Cet ordre est voulu : les textures (`Assets`) sont détruites avant le renderer, et SDL est fermé en tout dernier (`SdlContext` est déclaré en premier).

```cpp
SdlContext sdl_;       // construit en 1er, détruit en dernier
WindowPtr window_;
RendererPtr renderer_;
Assets assets_;        // les textures ont besoin du renderer
GameSession session_;
...
```

### Boucle à pas fixe (`runFrame`)

La logique du jeu avance toujours par pas de 1/60 s (`config::tickDuration`), quel que soit le nombre d'images par seconde. Le temps réel écoulé s'accumule dans `timeToSimulate_`, et on exécute autant de « ticks » que nécessaire pour le rattraper :

```cpp
timeToSimulate_ = std::min(timeToSimulate_ + elapsed, 0.25f);
while (running_ && timeToSimulate_ >= config::tickDuration) {
    currentScreen().update(config::tickDuration);
    applyScreenRequest();
    timeToSimulate_ -= config::tickDuration;
}
// puis on dessine une seule fois
```

- À 144 FPS, la plupart des images n'exécutent aucun tick, ou un seul.
- À 30 FPS, chaque image en exécute deux.
- Le plafond de 0,25 s évite de simuler d'un coup plusieurs secondes après une interruption (fenêtre déplacée, PC en veille…).

Résultat : le jeu va à la même vitesse sur toutes les machines, et la simulation reste reproductible.
L'affichage est synchronisé sur l'écran par la VSync (`SDL_RENDERER_PRESENTVSYNC`).

### Changement d'écran

Un écran ne change jamais l'écran courant lui-même : il *demande* un changement avec `goTo(ScreenId::...)`. Après chaque évènement et chaque tick, `Game::applyScreenRequest()` récupère la demande, appelle `onLeave()` sur l'ancien écran puis `onEnter()` sur le nouveau. `ScreenId::Quit` ferme le jeu.

Les écrans sont rangés dans un `std::array` indexé par `ScreenId` : ils sont tous créés au démarrage et gardent leur état.

### Plein écran

F11 est géré directement par `Game` (dans tous les écrans), en basculant sur `SDL_WINDOW_FULLSCREEN_DESKTOP`.

---

## `SdlContext` et utilitaires SDL (`sdlUtils.hpp/.cpp`)

### Libération automatique des ressources

Les objets SDL (fenêtre, textures, polices…) sont rangés dans des `std::unique_ptr` avec un « deleter » qui appelle la bonne fonction de destruction SDL. On n'écrit jamais `SDL_DestroyTexture` à la main.

```cpp
struct SdlDeleter {
    void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
    void operator()(TTF_Font* font) const { TTF_CloseFont(font); }
    ...
};
using TexturePtr = std::unique_ptr<SDL_Texture, SdlDeleter>;
```

### `SdlContext`

Initialise SDL, SDL_image et SDL_ttf dans son constructeur et les ferme dans son destructeur (principe RAII). Si une initialisation échoue, celles déjà faites sont annulées avant de lever l'exception. La classe n'est pas copiable : une copie fermerait SDL deux fois.

### `SdlError`

Exception qui ajoute automatiquement `SDL_GetError()` au message, pour savoir *pourquoi* SDL a échoué.

### Fonctions utilitaires

- `loadTexture` et `loadFont` chargent un fichier ou lèvent une `SdlError`.
- `executableDirectory()` renvoie le dossier de l'exécutable, avec `SDL_GetBasePath`.
- Les chemins sont convertis en UTF-8 avant d'être passés à SDL. C'est nécessaire sous Windows quand le chemin contient des accents.

---

## `Assets` (`assets.hpp/.cpp`)

Charge **une seule fois** toutes les images et polices du jeu. Les autres classes gardent une référence `const Assets&` et n'ont rien à libérer.

- **Recherche des dossiers** : `findRootDirectory()` cherche un dossier contenant `img/` et `fonts/` à côté de l'exécutable, dans son dossier parent, puis dans le dossier courant. C'est pour ça que le jeu fonctionne lancé depuis `build/` comme depuis `cmake-build-debug/`.
- **Animations** : une `Animation` est un `std::vector<TexturePtr>`. `loadAnimation` charge `prefix-1.png`, `prefix-2.png`… tant que les fichiers existent. Ajouter une image à une animation ne demande donc aucune modification du code.
- **Polices** : une seule police (`Retro_Gaming.ttf`) en trois tailles : 18, 32 et 60 pt.

---

## `GameSession` (`gameSession.hpp/.cpp`)

Une partie : le joueur, les poissons, les bonus, la caméra, le chrono et le score. Cette classe ne connaît que les règles du jeu : pas de clavier, pas de menus, pas de HUD.

### Démarrer une partie (`start`)

Remet tout à zéro : chrono à 60 s, décompte à 3 s, effets de bonus à 0, joueur au centre, puis remplit le monde de poissons et de bulles **en dehors de l'écran** (`noSpawnArea()`).

### Un tick de jeu (`update`)

Dans l'ordre :

1. Les poissons nagent et les bulles flottent, **même pendant le décompte**.
2. Pendant le décompte, le joueur ne bouge pas, et on s'arrête là.
3. Le joueur se déplace (plus vite si le sprint est actif), et la caméra le suit.
4. Les poissons touchés sont mangés : kills, score (doublé si le x2 est actif) et croissance de l'espadon.
5. Les bulles touchées appliquent leur effet (`applyBonus`).
6. Les poissons et les bulles manquants réapparaissent hors de l'écran.
7. Les chronos (partie, sprint, x2) diminuent de `dt`, sans descendre sous 0.

```cpp
const int pointsMultiplier = doubleScoreTimeLeft_ > 0.0f ? config::scoreMultiplier : 1;
kills_ += eaten.count;
score_ += eaten.points * pointsMultiplier;
player_.grow(eaten.count);
```

### Effets des bonus (`applyBonus`)

Chaque effet est un simple compteur de temps restant. **Les durées se cumulent** : reprendre un x2 alors qu'il lui reste 2 s le fait passer à 7 s.

```cpp
case BonusType::Sprint:
    sprintTimeLeft_ += config::bonusEffectDuration;
    break;
```

Comme les compteurs ne descendent jamais sous 0, le même `+=` marche aussi quand l'effet était inactif.

### Affichage (`draw`)

Dessine la partie visible de l'image de fond (le rectangle de la caméra, mis à l'échelle de la texture), puis les bulles, les poissons et le joueur, dans cet ordre (le dernier dessiné passe devant).

### Zone interdite à l'apparition (`noSpawnArea`)

C'est le rectangle de la caméra, agrandi de 60 px de chaque côté : rien n'apparaît sous les yeux du joueur.

---

## `Player` (`player.hpp/.cpp`)

L'espadon contrôlé par le joueur.

### Ce que demande le joueur : `PlayerInput`

```cpp
struct PlayerInput {
    Vec2 direction;     // {0, 0} = à l'arrêt
    bool dash = false;  // Espace appuyée depuis le tick précédent
};
```

### Déplacement (`update`)

- La direction est **normalisée** : en diagonale, on ne va pas plus vite qu'en ligne droite.
- La vitesse ne change pas d'un coup : elle se rapproche à chaque tick d'une fraction de la vitesse visée. On obtient des départs et des arrêts en douceur.

```cpp
const float blend = std::min(1.0f, config::playerAcceleration * dt);
velocity_ = velocity_ + (targetVelocity - velocity_) * blend;
```

- Le joueur est ensuite bloqué dans les limites du monde, en tenant compte de sa taille actuelle.
- `facing_` retient la dernière direction de nage. Elle sert au dash quand aucune touche n'est appuyée.

### Dash

Sur appui d'Espace, si la recharge est terminée : pendant `dashDuration` (0,25 s), la vitesse est **imposée** à 2,5 fois la vitesse normale dans la direction `facing_`. À la fin, le lissage habituel ramène le poisson à sa vitesse normale.
La recharge (`dashCooldown`, 2 s) se compte depuis le **début** du dash. `dashReadiness()` renvoie une valeur entre 0 et 1, utilisée par la jauge du HUD.

```cpp
if (input.dash && dashCooldownLeft_ <= 0.0f) {
    dashDirection_ = facing_;
    dashTimeLeft_ = config::dashDuration;
    dashCooldownLeft_ = config::dashCooldown;
}
```

### Croissance (`grow`)

Chaque poisson mangé augmente `scale_` de 0,8 %, jusqu'à 1,8 au maximum. L'échelle s'applique au sprite **et** à la hitbox : un gros espadon mange plus facilement.

### Affichage (`draw`)

- Il utilise l'animation de nage (en mouvement ou pendant un dash) ou d'attente (immobile).
- L'image est tournée dans le sens de la vitesse. Quand le poisson va vers la gauche (angle > 90°), elle est retournée verticalement pour qu'il ne nage pas sur le dos.

### Hitbox

C'est un carré centré sur le poisson. Un carré reste valable quelle que soit l'orientation de l'espadon, sans calcul de rotation.

---

## `BoidSimulation` (`boidSimulation.hpp/.cpp`)

Les 1000 poissons, qui nagent en bancs grâce à l'algorithme des **boids**.

### Les données

Chaque poisson est une petite structure (`Boid`) : position, vitesse (en pixels **par tick**), espèce et étape d'animation. Les trois espèces (`speciesTable`) diffèrent par leur taille et leurs points (1, 2 ou 3).

### Les règles des boids (`steer`)

Chaque tick, un poisson regarde ses voisins et ajuste sa vitesse :

| Règle | Effet | Réglage |
| --- | --- | --- |
| Séparation | s'écarte des voisins à moins de 40 px | `avoidFactor` |
| Cohésion | va vers le centre des voisins visibles (< 100 px) | `centeringFactor` |
| Alignement | prend la vitesse moyenne de ces voisins | `matchingFactor` |
| Fuite | s'éloigne du joueur s'il est à moins de 160 px | `fleeFactor` |
| Bords | fait demi-tour progressivement près des bords du monde | `turnFactor` |

La vitesse est enfin bornée entre `minSpeed` et `maxSpeed`. Un poisson à l'arrêt complet est relancé.

### La grille spatiale (`rebuildGrid`)

Comparer chaque poisson à tous les autres ferait 1000 × 1000 = 1 million de tests par tick.
Le monde est donc découpé en cases de 100 px (la distance de vue). Un poisson ne regarde que **les 9 cases autour de lui**.

La grille est reconstruite à chaque tick par un **tri par comptage**, sans allocation :

```cpp
// 1. Combien de poissons par case, puis où commence chaque case.
for (const Boid& boid : boids_) cellStart_[cellOf(boid) + 1] += 1;
for (int cell = 0; cell < cellCount; ++cell) cellStart_[cell + 1] += cellStart_[cell];

// 2. Chaque indice de poisson est rangé dans la tranche de sa case.
cellCursor_ = cellStart_;
for (std::size_t index = 0; index < boids_.size(); ++index) {
    const int cell = cellOf(boids_[index]);
    boidsByCell_[cellCursor_[cell]++] = static_cast<int>(index);
}
```

Les poissons de la case `c` sont ensuite `boidsByCell_[cellStart_[c]]` à `boidsByCell_[cellStart_[c + 1] - 1]`.

### Manger et réapparaître

- `eatTouching(hitbox)` retire les poissons qui touchent la hitbox (`std::erase_if`) et renvoie leur nombre et leurs points.
- `respawnMissing(zone)` complète jusqu'à 1000 poissons, en tirant des positions au hasard jusqu'à en trouver une **hors** de la zone visible.
- Chaque poisson démarre à une étape d'animation aléatoire, pour qu'ils ne battent pas tous des nageoires en même temps.

### Affichage

Seuls les poissons visibles à l'écran sont dessinés. Leur orientation se calcule comme pour le joueur.

---

## `BonusField` (`bonuses.hpp/.cpp`)

Les 6 bulles bonus réparties dans le monde.

| Type (`BonusType`) | Bulle | Effet (appliqué par `GameSession`) |
| --- | --- | --- |
| `ExtraTime` | `+5` | +5 s au chrono |
| `Sprint` | `>>` | vitesse × 1,5 pendant 5 s |
| `DoubleScore` | `x2` | points × 2 pendant 5 s |

- **Séparation des rôles** : `BonusField` ne sait pas ce que fait un bonus. `takeTouching(hitbox)` retire les bulles touchées et renvoie leurs types, et c'est `GameSession` qui applique les effets.
- **Apparition** : même principe que les poissons. Les bulles apparaissent au hasard, jamais à l'écran. Une nouvelle bulle remplace aussitôt chaque bulle prise.
- **Flottement** : chaque bulle monte et descend selon un sinus. Chacune a sa propre `phase` aléatoire, pour qu'elles ne bougent pas toutes ensemble.

```cpp
const float bob = std::sin(time_ * bobSpeed + bonus.phase) * bobHeight;
```

- **Étiquettes** : les trois textes (`+5`, `>>`, `x2`) sont créés une fois dans le constructeur, puis réutilisés à chaque image.

---

## `Camera` (`camera.hpp`)

La partie du monde visible à l'écran, repérée par son coin haut-gauche.

- `centerOn(cible)` centre la vue sur le joueur, **sans sortir du monde** : près d'un bord, la caméra s'arrête et c'est le joueur qui se décale à l'écran. La position est arrondie au pixel pour éviter que le fond « tremble ».
- Pour dessiner un objet : `position à l'écran = position dans le monde - position de la caméra`.

---

## `geometry.hpp`

Les petits outils mathématiques utilisés partout :

- `Vec2` : vecteur 2D avec `+`, `-`, `* float` et `length()` ;
- `angleInDegrees(v)` : orientation d'un vecteur, dans le sens attendu par `SDL_RenderCopyEx` ;
- `overlaps(a, b)` : est-ce que deux rectangles se touchent (collisions, objets visibles à l'écran) ;
- `contains(rect, point)` : est-ce qu'un point est dans un rectangle (zone d'apparition) ;
- `centeredRect(centre, w, h)` : rectangle centré sur un point.

---

## `HighScores` (`highScores.hpp/.cpp`)

L'historique des parties, enregistré dans `scoreDB.txt` à côté de l'exécutable.

- **Format** : une ligne par partie, `nom;kills;score`, la plus récente en premier. C'est pour ça que `;` est interdit dans les pseudos.
- **Lecture robuste** : une ligne mal formée (fichier modifié à la main…) est ignorée au lieu de faire planter le jeu. `parseLine` renvoie un `std::optional` vide dans ce cas.
- `add` insère la partie en tête et réécrit le fichier, `clear` vide l'historique.
- Le fichier garde **toutes** les parties. Le tri pour le classement se fait à l'affichage (voir `ScoresScreen`).

---

## `Text` (`text.hpp/.cpp`)

Un texte à l'écran. Avec SDL_ttf, transformer un texte en image coûte cher. `Text` garde donc la texture et ne la recalcule que si le contenu ou la couleur change réellement :

```cpp
timer_.set(std::to_string(seconds));  // appelé à chaque image, mais
                                      // la texture n'est refaite qu'une fois par seconde
```

Trois façons de le dessiner : `draw` (coin haut-gauche), `drawCentered` (centré horizontalement) et `drawRightAligned` (aligné à droite, pour le chrono et les colonnes de nombres).

---

## `Button` (`button.hpp/.cpp`)

Un bouton : une image de fond et un libellé centré. `contains(point)` sert à détecter les clics. Quand la souris n'est pas dessus, le fond est légèrement assombri (`SDL_SetTextureColorMod`). Le bouton ne sait pas ce qu'il déclenche : c'est l'écran qui associe une action à chaque bouton.

---

## Les écrans (`screens/`)

### `Screen` : la classe de base (`screen.hpp/.cpp`)

Chaque écran hérite de `Screen` et redéfinit les méthodes dont il a besoin :

```cpp
virtual void onEnter() {}                         // l'écran devient l'écran courant
virtual void onLeave() {}                         // on le quitte
virtual void handleEvent(const SDL_Event& event); // clavier, souris
virtual void update(float dt) {}                  // un tick de logique
virtual void draw() = 0;                          // une image
```

Ce que `Screen` fournit à tous les écrans :

- **Boutons** : `addButton(libellé, zone, action)` associe une fonction (souvent une lambda) au bouton. `Screen::handleEvent` gère le survol et le clic.

  ```cpp
  addButton("HOME", areas[0], [this] { goTo(ScreenId::Welcome); });
  ```

- **Navigation** : `goTo(id)` demande un changement d'écran, que `Game` applique ensuite.
- **Mise en page** : `drawLogo`, `titleBottom`, `buttonArea`, `buttonRow` (boutons côte à côte, centrés), `drawMenuBackground`, `dimScreen` (voile sombre).
- **`GameContext`** : ce que les écrans partagent (renderer, assets, partie, scores), passé par référence à leur création.

### `WelcomeScreen`

Le logo et trois boutons empilés : START (vers la saisie du pseudo), SCORE et QUIT.

### `NameEntryScreen`

Saisie du pseudo avant la partie.

- La saisie de texte SDL (`SDL_StartTextInput`) n'est activée que sur cet écran, dans `onEnter`/`onLeave`.
- Le texte arrive par `SDL_TEXTINPUT`, ce qui respecte la disposition du clavier (AZERTY/QWERTY). Seuls les caractères ASCII imprimables sauf `;` sont acceptés, jusqu'à 16 caractères.
- Le dernier pseudo utilisé est proposé par défaut. Un `_` sert de curseur.
- Entrée lance la partie (`session.start(nom)`), Échap revient à l'accueil.

### `PlayingScreen`

La partie en cours et son interface (HUD).

- **Clavier** : `readDirection()` lit l'état des touches à chaque tick avec des **scancodes** (position physique). `SDL_SCANCODE_W` correspond donc à Z sur AZERTY et à W sur QWERTY : ZQSD et WASD marchent sans réglage.
- **Dash** : l'appui sur Espace arrive par un évènement, entre deux ticks. Il est mémorisé dans `dashPressed_`, transmis au tick suivant, puis effacé : un appui déclenche un seul dash. `onEnter` le remet à `false` pour éviter un dash « fantôme » au retour de pause.
- **Fin de partie** : quand le chrono atteint 0, la partie est enregistrée dans `HighScores` et on passe à `GameOverScreen`.
- **Pause** : Échap, ou la fenêtre qui perd le focus.
- **HUD** : kills et score en haut à gauche, effets actifs dessous (« SPRINT 4 », « SCORE x2 7 »), chrono en haut à droite (rouge sous 10 s), jauge de dash en bas à gauche (blanche en recharge, verte quand le dash est prêt). Pendant le décompte, l'écran est assombri et affiche 3, 2, 1.
- Les secondes affichées sont arrondies **au-dessus** : il reste 2,3 s, on affiche « 3 ».

### `PausedScreen`

La partie reste dessinée en arrière-plan, figée (elle n'est plus mise à jour), sous un voile sombre. Boutons RESUME, HOME, QUIT ; Échap reprend la partie.

### `GameOverScreen`

Affiche le score et les kills de la partie terminée (préparés dans `onEnter`). RE-START relance directement une partie avec le même pseudo.

### `ScoresScreen`

Le **top 10** des meilleures parties, en tableau.

- **Classement** : les parties sont triées par score, puis par kills en cas d'égalité. `std::stable_sort` garde la partie la plus récente devant en cas d'égalité parfaite.
- **Colonnes** : chaque cellule est un `Text` séparé, rangé dans une structure `Row`. Le rang et le nom sont alignés à gauche, le score et les kills à droite (`drawRightAligned`). C'est indispensable avec une police dont les caractères n'ont pas tous la même largeur.

  ```cpp
  row.rank.draw(screenCenterX + rankLeft, y);
  row.name.draw(screenCenterX + nameLeft, y);
  row.score.drawRightAligned(screenCenterX + scoreRight, y);
  row.kills.drawRightAligned(screenCenterX + killsRight, y);
  ```

- **Podium** : les trois premières lignes sont en or, argent et bronze (couleurs dans `config.hpp`).
- **Performance** : le tableau n'est reconstruit que dans `onEnter` et après DELETE, pas à chaque image.

---

## `config.hpp`

Tous les réglages au même endroit, en `inline constexpr` : fenêtre, monde, pas de temps, durée de partie, vitesse et dash du joueur, croissance, nombre de poissons et de bulles, effets des bonus, scores, couleurs de l'interface. Pour équilibrer le jeu, c'est le seul fichier à modifier dans la plupart des cas.

Les réglages fins du comportement des boids (distances, forces) sont, eux, des constantes privées de `BoidSimulation`, car ils ne concernent que cette classe.
