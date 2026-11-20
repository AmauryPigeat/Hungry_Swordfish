#include "sdlUtils.hpp"

namespace {

// SDL attend des chemins en UTF-8, y compris sous Windows.
std::string toUtf8(const std::filesystem::path& path) {
    const auto text = path.u8string();
    return {text.begin(), text.end()};
}

}  // namespace

SdlError::SdlError(const std::string& what) : std::runtime_error(what + " : " + SDL_GetError()) {}

SdlContext::SdlContext() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        throw SdlError("Initialisation de SDL impossible");
    }
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
        SDL_Quit();
        throw SdlError("Initialisation de SDL_image impossible");
    }
    if (TTF_Init() != 0) {
        IMG_Quit();
        SDL_Quit();
        throw SdlError("Initialisation de SDL_ttf impossible");
    }
}

SdlContext::~SdlContext() {
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

TexturePtr loadTexture(SDL_Renderer* renderer, const std::filesystem::path& file) {
    TexturePtr texture(IMG_LoadTexture(renderer, toUtf8(file).c_str()));
    if (!texture) {
        throw SdlError("Chargement de l'image " + file.string() + " impossible");
    }
    return texture;
}

FontPtr loadFont(const std::filesystem::path& file, int size) {
    FontPtr font(TTF_OpenFont(toUtf8(file).c_str(), size));
    if (!font) {
        throw SdlError("Chargement de la police " + file.string() + " impossible");
    }
    return font;
}

std::filesystem::path executableDirectory() {
    char* basePath = SDL_GetBasePath();  // en UTF-8, terminé par un séparateur
    if (basePath == nullptr) {
        return std::filesystem::current_path();
    }
    const std::string text(basePath);
    SDL_free(basePath);

    const std::filesystem::path directory(std::u8string(text.begin(), text.end()));
    // "dossier/" -> "dossier" : parent_path() retire le séparateur final.
    if (directory.has_filename()) {
        return directory;
    }
    return directory.parent_path();
}
