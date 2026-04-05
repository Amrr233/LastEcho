#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Data.h"

// ==============================
// STRUCTS
// ==============================
struct MapLayer {
    std::vector<int> tiles;
};

struct Map {
    std::vector<MapLayer> layers;
    sf::Texture           tileset;
    int                   widthTiles;
    int                   heightTiles;
    int                   tileSize;
};

struct Portal {
    sf::FloatRect      bounds;
    std::string        targetMapFolder;
    std::string        targetTileset;
    std::vector<std::string> targetLayers;
    int                targetWidth;
    int                targetHeight;
    int                targetTileSize;
    sf::Vector2f       spawnPos;
};

// ==============================
// GLOBALS
// ==============================
extern Map                 currentMap;
extern std::vector<Portal> portals;
extern bool                isFading;

// ==============================
// MAP
// ==============================
void loadMap(
    const std::string&              tilesetPath,
    const std::vector<std::string>& layerPaths,
    int widthTiles,
    int heightTiles,
    int tileSize
);

void drawMap(sf::RenderWindow& window);

// ==============================
// PORTAL
// ==============================
void addPortal(
    sf::FloatRect                   bounds,
    const std::string&              targetMapFolder,
    const std::string&              targetTileset,
    const std::vector<std::string>& targetLayers,
    int                             targetWidth,
    int                             targetHeight,
    int                             targetTileSize,
    sf::Vector2f                    spawnPos
);

void checkPortals(sf::RenderWindow& window);

// ==============================
// CAMERA
// ==============================
sf::View updateCamera(const Player& player, const Map& map);