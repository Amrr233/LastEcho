#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Data.h"

struct MapLayer { std::vector<int> tiles; };

struct Map {
    std::vector<MapLayer> layers;
    sf::Texture           tileset;
    int                   widthTiles = 0;
    int                   heightTiles = 0;
    int                   tileSize = 16;
};

struct Portal {
    sf::RectangleShape box;
    std::string ts, g, s, o;
    int targetW, targetTS;
    sf::Vector2f spawn;
};

extern Map currentMap;
extern std::vector<Portal> portals;
extern std::vector<int>    dugTiles;

void loadMap(const std::string& ts, const std::string& g, const std::string& s, const std::string& o, int w, int tSize);
void drawMap(sf::RenderWindow& window);
void addPortal(int x, int y, int w, int h, std::string ts, std::string g, std::string s, std::string o, int mw, int tsiz, sf::Vector2f spawn);
bool isSolid(float x, float y);
sf::View updateCamera(const Player& player, const Map& map);