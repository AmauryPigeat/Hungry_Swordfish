#!/usr/bin/env bash
# Vérifie les dépendances, compile et lance Hungry Swordfish (WSL / Debian 13).
#
# Usage : ./run.sh [--debug] [--clean] [--no-update] [--no-run]
#   --debug      compile en Debug au lieu de Release
#   --clean      supprime le dossier de build avant de compiler
#   --no-update  ne vérifie pas les mises à jour des paquets (plus rapide, hors ligne)
#   --no-run     compile sans lancer le jeu

set -euo pipefail

BUILD_TYPE=Release
CLEAN=0
CHECK_UPDATES=1
RUN=1

for arg in "$@"; do
    case "$arg" in
        --debug)     BUILD_TYPE=Debug ;;
        --clean)     CLEAN=1 ;;
        --no-update) CHECK_UPDATES=0 ;;
        --no-run)    RUN=0 ;;
        -h|--help)   sed -n '2,9p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
        *) echo "Option inconnue : $arg (voir --help)" >&2; exit 1 ;;
    esac
done

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
EXE="$BUILD_DIR/hungry_swordfish"

PACKAGES=(build-essential cmake pkg-config libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev)

info() { printf '\033[1;34m==>\033[0m %s\n' "$*"; }
ok()   { printf '\033[1;32m  ✓\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m  !\033[0m %s\n' "$*"; }

# ---------------------------------------------------------------- Paquets
info "Vérification des paquets"

# Rafraîchit l'index apt s'il date de plus d'un jour (sinon "à jour" ne veut rien dire).
if (( CHECK_UPDATES )); then
    lists=/var/lib/apt/lists
    if [[ -z "$(find "$lists" -maxdepth 1 -name '*Packages*' -mmin -1440 -print -quit 2>/dev/null)" ]]; then
        info "Index apt ancien, mise à jour (sudo apt-get update)"
        sudo apt-get update -qq
    fi
fi

to_install=()
for pkg in "${PACKAGES[@]}"; do
    installed="$(dpkg-query -W -f='${Status} ${Version}' "$pkg" 2>/dev/null || true)"
    if [[ "$installed" != "install ok installed "* ]]; then
        warn "$pkg : absent"
        to_install+=("$pkg")
        continue
    fi
    version="${installed##* }"
    if (( CHECK_UPDATES )); then
        candidate="$(apt-cache policy "$pkg" | awk '/Candidate:/ {print $2}')"
        if [[ -n "$candidate" && "$candidate" != "(none)" && "$candidate" != "$version" ]]; then
            warn "$pkg : $version -> $candidate disponible"
            to_install+=("$pkg")
            continue
        fi
    fi
    ok "$pkg ($version)"
done

if (( ${#to_install[@]} )); then
    info "Installation / mise à jour : ${to_install[*]}"
    sudo apt-get install -y "${to_install[@]}"
fi

# ---------------------------------------------------------------- Build
cd "$PROJECT_DIR"

if (( CLEAN )); then
    info "Suppression de $BUILD_DIR"
    rm -rf "$BUILD_DIR"
fi

# Un cache CMake créé depuis un autre dossier (ou sous Windows) fait échouer la configuration :
# on ne garde le cache que s'il correspond à ce dossier.
cache="$BUILD_DIR/CMakeCache.txt"
if [[ -f "$cache" ]]; then
    cached_src="$(sed -n 's/^CMAKE_HOME_DIRECTORY:INTERNAL=//p' "$cache")"
    if [[ "$cached_src" != "$PROJECT_DIR" ]]; then
        warn "Cache CMake créé pour un autre dossier ($cached_src), reconfiguration"
        rm -rf "$cache" "$BUILD_DIR/CMakeFiles"
    fi
fi

info "Configuration ($BUILD_TYPE)"
cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" >/dev/null

info "Compilation"
cmake --build "$BUILD_DIR" --parallel "$(nproc)"
ok "Exécutable : $EXE"

# ---------------------------------------------------------------- Lancement
(( RUN )) || exit 0

if [[ -z "${DISPLAY:-}" && -z "${WAYLAND_DISPLAY:-}" ]]; then
    warn "Aucun affichage détecté (DISPLAY / WAYLAND_DISPLAY vides)."
    warn "WSLg est-il actif ? Essayez 'wsl --update' puis 'wsl --shutdown' côté Windows."
    exit 1
fi

info "Lancement"
cd "$BUILD_DIR"   # le jeu trouve img/ et fonts/ dans le dossier parent
exec "$EXE"
