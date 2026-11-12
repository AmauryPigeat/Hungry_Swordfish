#include "highScores.hpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

namespace {

constexpr char separator = ';';

// Renvoie rien si la ligne est mal formée (fichier modifié à la main, etc.).
std::optional<ScoreEntry> parseLine(const std::string& line) {
    std::istringstream stream(line);
    ScoreEntry entry;
    std::string kills;
    std::string score;

    if (!std::getline(stream, entry.name, separator) || !std::getline(stream, kills, separator) ||
        !std::getline(stream, score, separator)) {
        return std::nullopt;
    }
    try {
        entry.kills = std::stoi(kills);
        entry.score = std::stoi(score);
    } catch (const std::exception&) {
        return std::nullopt;
    }
    return entry;
}

}  // namespace

HighScores::HighScores(std::filesystem::path file) : file_(std::move(file)) {
    load();
}

void HighScores::add(const ScoreEntry& entry) {
    entries_.insert(entries_.begin(), entry);
    save();
}

void HighScores::clear() {
    entries_.clear();
    save();
}

void HighScores::load() {
    entries_.clear();
    std::ifstream input(file_);
    std::string line;
    while (std::getline(input, line)) {
        if (const auto entry = parseLine(line)) {
            entries_.push_back(*entry);
        }
    }
}

void HighScores::save() const {
    std::ofstream output(file_, std::ios::trunc);
    if (!output) {
        std::cerr << "Impossible d'écrire les scores dans " << file_.string() << '\n';
        return;
    }
    for (const ScoreEntry& entry : entries_) {
        output << entry.name << separator << entry.kills << separator << entry.score << '\n';
    }
}
