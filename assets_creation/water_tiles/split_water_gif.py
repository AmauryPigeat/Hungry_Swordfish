"""Découpe img/Water_Item_Animated.gif en une image PNG par frame, dans img/Water/.

Chaque frame du GIF a un cadre transparent de 2 px tout autour : on le retire,
sinon les tuiles collées côte à côte laisseraient une grille de lignes vides.
Le GIF fait 300 x 300, chaque PNG fait donc 296 x 296 (config::waterTileSize).

Les fichiers sont nommés Water-1.png, Water-2.png... pour être chargés par
loadAnimation (src/assets.cpp), comme les animations des poissons.

Usage, depuis la racine du projet :
    python3 assets_creation/water_tiles/split_water_gif.py
Nécessite Pillow : sudo apt install python3-pil   (ou : pip install pillow)
"""

from pathlib import Path

from PIL import Image

PROJECT_ROOT = Path(__file__).resolve().parents[2]
SOURCE = "Water_Item_Animated.gif"
DESTINATION = PROJECT_ROOT / "img" / "Water"
BORDER = 2  # largeur du cadre transparent à retirer, en pixels


def main():
    DESTINATION.mkdir(exist_ok=True)

    # On supprime les anciennes images, au cas où le nouveau GIF aurait moins de frames.
    for old_file in DESTINATION.glob("Water-*.png"):
        old_file.unlink()

    gif = Image.open(SOURCE)
    width, height = gif.size
    crop_box = (BORDER, BORDER, width - BORDER, height - BORDER)

    for index in range(gif.n_frames):
        gif.seek(index)
        # convert("RGBA") donne la frame complète, même si le GIF ne stocke que ce qui change.
        frame = gif.convert("RGBA").crop(crop_box)
        frame.save(DESTINATION / f"Water-{index + 1}.png")

    print(f"{gif.n_frames} images de {width - 2 * BORDER} x {height - 2 * BORDER} écrites dans {DESTINATION}")


if __name__ == "__main__":
    main()
