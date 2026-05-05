#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <vector>
#include <string>
#include <map>
using namespace std;
using namespace sf;
using json = nlohmann::json;

struct Portal {
    sf::FloatRect bounds;
    std::string targetMap;
    sf::Vector2f spawnPos;
};


struct MapLayer {
    std::vector<int> data;
    std::string name;
    bool visible;
};

struct GameMap {
    int width = 0;
    int height = 0;
    int tileSize = 0;
    sf::Texture tilesetTexture;
    vector<MapLayer> layers;
    vector<Portal> portals;
    std::map<int, std::map<std::string, std::string>> tileProperties;
};

bool loadMapFromJSON(GameMap& map, const std::string& jsonPath);
void drawMap(sf::RenderWindow& window, const GameMap& map);
View updateMapView(View& currentView, const GameMap& map, Vector2f playerPos, float deltaTime);
inline int getMapWidthPixels(const GameMap& map) { return map.width * map.tileSize; }
inline int getMapHeightPixels(const GameMap& map) { return map.height * map.tileSize; }
bool mapIsWalkable(const GameMap& map, float x, float y);
bool mapCheckCollision(const GameMap& map, sf::FloatRect playerBounds, const std::string& mapName);
bool mapSwapTileset(GameMap& map, const std::string& newTexturePath);
#endif