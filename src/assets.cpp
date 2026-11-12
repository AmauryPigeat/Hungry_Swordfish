#include "assets.hpp"

#include <string>

namespace fs = std::filesystem;

namespace {

// Charge prefix-1.png, prefix-2.png, ... tant que les fichiers existent.
Animation loadAnimation(SDL_Renderer* renderer, const fs::path& directory,
                        const std::string& prefix) {
    Animation frames;
    for (int number = 1;; ++number) {
        const fs::path file = directory / (prefix + "-" + std::to_string(number) + ".png");
        if (!fs::exists(file)) {
            break;
        }
        frames.push_back(loadTexture(renderer, file));
    }
    if (frames.empty()) {
        throw std::runtime_error("Aucune image d'animation dans " + directory.string());
    }
    return frames;
}

}  // namespace

Assets::Assets(SDL_Renderer* renderer) {
    const fs::path root = findRootDirectory();
    const fs::path images = root / "img";
    const fs::path fontFile = root / "fonts" / "Retro_Gaming.ttf";

    background = loadTexture(renderer, images / "background_image_4k.png");
    logo = loadTexture(renderer, images / "swordfish_game_logo.png");
    button = loadTexture(renderer, images / "button_background.png");
    deleteButton = loadTexture(renderer, images / "button_delete_background.png");

    playerSwim = loadAnimation(renderer, images / "Walk_player", "Walk_player");
    playerIdle = loadAnimation(renderer, images / "Idle_player", "Idle_player");
    for (std::size_t species = 0; species < fish.size(); ++species) {
        const fs::path directory = images / ("Walk_fish_" + std::to_string(species + 1));
        fish[species] = loadAnimation(renderer, directory, "Walk_fish");
    }

    smallFont = loadFont(fontFile, 18);
    mediumFont = loadFont(fontFile, 32);
    largeFont = loadFont(fontFile, 60);
}

fs::path Assets::findRootDirectory() {
    const fs::path executable = executableDirectory();
    const fs::path current = fs::current_path();
    const fs::path candidates[] = {executable, executable.parent_path(), current,
                                   current.parent_path()};

    for (const fs::path& candidate : candidates) {
        if (fs::is_directory(candidate / "img") && fs::is_directory(candidate / "fonts")) {
            return candidate;
        }
    }
    throw std::runtime_error("Dossiers img/ et fonts/ introuvables (cherchés depuis " +
                             executable.string() + ")");
}
