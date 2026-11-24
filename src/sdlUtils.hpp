#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>

// Erreur levée quand un appel SDL échoue. Le message contient SDL_GetError().
class SdlError : public std::runtime_error {
public:
    explicit SdlError(const std::string& what);
};

// Les ressources SDL sont rangées dans des unique_ptr : elles sont libérées
// automatiquement, plus besoin d'écrire de destructeurs à la main.
struct SdlDeleter {
    void operator()(SDL_Window* window) const { SDL_DestroyWindow(window); }
    void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
    void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
    void operator()(SDL_Surface* surface) const { SDL_FreeSurface(surface); }
    void operator()(TTF_Font* font) const { TTF_CloseFont(font); }
};

using WindowPtr = std::unique_ptr<SDL_Window, SdlDeleter>;
using RendererPtr = std::unique_ptr<SDL_Renderer, SdlDeleter>;
using TexturePtr = std::unique_ptr<SDL_Texture, SdlDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SdlDeleter>;
using FontPtr = std::unique_ptr<TTF_Font, SdlDeleter>;

// Initialise SDL, SDL_image et SDL_ttf, et les ferme à la destruction.
class SdlContext {
public:
    SdlContext();
    ~SdlContext();
    SdlContext(const SdlContext&) = delete;
    SdlContext& operator=(const SdlContext&) = delete;
};

TexturePtr loadTexture(SDL_Renderer* renderer, const std::filesystem::path& file);
SurfacePtr loadSurface(const std::filesystem::path& file);  // image en mémoire, pas sur la carte graphique
FontPtr loadFont(const std::filesystem::path& file, int size);

// Dossier qui contient l'exécutable.
std::filesystem::path executableDirectory();
