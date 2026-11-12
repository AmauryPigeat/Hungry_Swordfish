#pragma once

#include <filesystem>
#include <string>
#include <vector>

struct ScoreEntry {
    std::string name;
    int kills = 0;
    int score = 0;
};

// Historique des parties, sauvegardé dans un fichier texte.
// Une ligne par partie, la plus récente en premier : nom;kills;score
class HighScores {
public:
    explicit HighScores(std::filesystem::path file);

    const std::vector<ScoreEntry>& entries() const { return entries_; }

    void add(const ScoreEntry& entry);
    void clear();

private:
    void load();
    void save() const;

    std::filesystem::path file_;
    std::vector<ScoreEntry> entries_;
};
