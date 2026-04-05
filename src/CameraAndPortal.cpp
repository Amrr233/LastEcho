#include "CameraAndPortal.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <player.h>
using namespace sf;
using namespace std;

// ==============================
// GLOBALS
// ==============================
Map               currentMap;
vector<Portal>    portals;
bool              isFading = false;

extern Player      player;
extern RenderWindow window;

// ==============================
// HELPERS
// ==============================
static vector<int> loadCSV(const string& path) {
    vector<int> data;
    ifstream f(path);
    if (!f.is_open()) {
        cout << "Error: Cannot open " << path << endl;
        return data;
    }
    string line, word;
    while (getline(f, line)) {
        stringstream ss(line);
        while (getline(ss, word, ',')) {
            word.erase(remove(word.begin(), word.end(), '\r'), word.end());
            if (!word.empty())
                data.push_back(stoi(word));
        }
    }
    return data;
}

static void drawLayer(
    sf::RenderWindow&  window,
    const vector<int>& tiles,
    const sf::Texture& tileset,
    int                widthTiles,
    int                tileSize)
{
    int tilesPerRow = tileset.getSize().x / tileSize;
    for (int i = 0; i < (int)tiles.size(); i++) {
        int id = tiles[i];
        if (id == -1) continue;
        sf::Sprite tile(tileset);
        tile.setTextureRect(IntRect(
            (id % tilesPerRow) * tileSize,
            (id / tilesPerRow) * tileSize,
            tileSize, tileSize
        ));
        tile.setPosition(
            (i % widthTiles) * tileSize,
            (i / widthTiles) * tileSize
        );
        window.draw(tile);
    }
}

// ==============================
// MAP — بيحمل ويرسم بس
// ==============================
void loadMap(
    const string&         tilesetPath,
    const vector<string>& layerPaths,
    int widthTiles,
    int heightTiles,
    int tileSize)
{
    currentMap.layers.clear();
    portals.clear();

    currentMap.widthTiles  = widthTiles;
    currentMap.heightTiles = heightTiles;
    currentMap.tileSize    = tileSize;

    if (!currentMap.tileset.loadFromFile(tilesetPath))
        cout << "Error: Cannot load tileset " << tilesetPath << endl;

    // بيحمل كل الـ layers بالترتيب — مش بيفكر في collision
    for (const string& path : layerPaths) {
        MapLayer layer;
        layer.tiles = loadCSV(path);
        currentMap.layers.push_back(layer);
    }
}

void drawMap(sf::RenderWindow& window) {
    for (const MapLayer& layer : currentMap.layers)
        drawLayer(window, layer.tiles, currentMap.tileset,
                  currentMap.widthTiles, currentMap.tileSize);
}

// ==============================
// PORTAL
// ==============================
void addPortal(
    FloatRect                   bounds,
    const string&               targetMapFolder,
    const string&               targetTileset,
    const vector<string>&       targetLayers,
    int                         targetWidth,
    int                         targetHeight,
    int                         targetTileSize,
    Vector2f                    spawnPos)
{
    Portal p;
    p.bounds          = bounds;
    p.targetMapFolder = targetMapFolder;
    p.targetTileset   = targetTileset;
    p.targetLayers    = targetLayers;
    p.targetWidth     = targetWidth;
    p.targetHeight    = targetHeight;
    p.targetTileSize  = targetTileSize;
    p.spawnPos        = spawnPos;
    portals.push_back(p);
}

void checkPortals(sf::RenderWindow& window) {
    if (isFading) return;

    FloatRect playerBounds(player.pos.x - 8, player.pos.y - 8, 16, 16);

    for (const Portal& p : portals) {
        if (!playerBounds.intersects(p.bounds)) continue;

        // Fade out
        RectangleShape fade(Vector2f(SCREEN_W, SCREEN_H));
        isFading = true;

        for (int alpha = 0; alpha <= 255; alpha += 5) {
            window.clear(Color::Black);
            drawMap(window);
            drawPlayer(window);
            fade.setFillColor(Color(0, 0, 0, alpha));
            View old = window.getView();
            window.setView(window.getDefaultView());
            window.draw(fade);
            window.setView(old);
            window.display();
            sf::sleep(sf::milliseconds(16));
        }

        // Load new map
        vector<string> fullPaths;
        for (const string& l : p.targetLayers)
            fullPaths.push_back(p.targetMapFolder + "/" + l);

        loadMap(
            p.targetMapFolder + "/" + p.targetTileset,
            fullPaths,
            p.targetWidth,
            p.targetHeight,
            p.targetTileSize
        );

        player.pos = p.spawnPos;

        // Fade in
        for (int alpha = 255; alpha >= 0; alpha -= 5) {
            window.clear(Color::Black);
            drawMap(window);
            drawPlayer(window);
            fade.setFillColor(Color(0, 0, 0, alpha));
            View old = window.getView();
            window.setView(window.getDefaultView());
            window.draw(fade);
            window.setView(old);
            window.display();
            sf::sleep(sf::milliseconds(16));
        }

        isFading = false;
        break;
    }
}

// ==============================
// CAMERA
// ==============================
sf::View updateCamera(const Player& player, const Map& map) {
    float mapW  = map.widthTiles  * map.tileSize;
    float mapH  = map.heightTiles * map.tileSize;
    float halfW = SCREEN_W / 2.0f;
    float halfH = SCREEN_H / 2.0f;

    float camX = player.pos.x;
    float camY = player.pos.y;

    if (camX - halfW < 0)    camX = halfW;
    if (camY - halfH < 0)    camY = halfH;
    if (camX + halfW > mapW) camX = mapW - halfW;
    if (camY + halfH > mapH) camY = mapH - halfH;

    return sf::View(Vector2f(camX, camY), Vector2f(SCREEN_W, SCREEN_H));
}