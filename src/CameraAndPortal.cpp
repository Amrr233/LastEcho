#include "CameraAndPortal.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

Map currentMap;
std::vector<Portal> portals;
std::vector<int> dugTiles;

void loadMap(const std::string& ts, const std::string& g, const std::string& s, const std::string& o, int w, int tSize) {
    currentMap.layers.clear();
    portals.clear();
    dugTiles.clear();

    if (!currentMap.tileset.loadFromFile(ts)) return;

    currentMap.widthTiles = w;
    currentMap.tileSize = tSize;

    auto loadCSV = [](const std::string& path) {
        std::vector<int> data;
        std::ifstream file(path);
        std::string line, val;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            while (std::getline(ss, val, ',')) {
                if (!val.empty()) data.push_back(std::stoi(val));
            }
        }
        return data;
    };

    MapLayer l1, l2, l3;
    l1.tiles = loadCSV(g); l2.tiles = loadCSV(s); l3.tiles = loadCSV(o);

    currentMap.layers.push_back(l1);
    currentMap.layers.push_back(l2);
    currentMap.layers.push_back(l3);

    // استنتاج الارتفاع بناءً على العرض المدخل (60)
    if (w > 0 && !l1.tiles.empty())
        currentMap.heightTiles = (int)l1.tiles.size() / w;
}

void drawMap(sf::RenderWindow& window) {
    if (currentMap.layers.empty()) return;
    sf::Sprite tile(currentMap.tileset);
    int tilesPerRow = currentMap.tileset.getSize().x / currentMap.tileSize;

    for (size_t L = 0; L < currentMap.layers.size(); L++) {
        for (int i = 0; i < (int)currentMap.layers[L].tiles.size(); i++) {
            int id = currentMap.layers[L].tiles[i];
            if (id < 0) continue;

            int tx = (id % tilesPerRow) * currentMap.tileSize;
            int ty = (id / tilesPerRow) * currentMap.tileSize;

            tile.setTextureRect(sf::IntRect(tx, ty, currentMap.tileSize, currentMap.tileSize));
            tile.setPosition((float)(i % currentMap.widthTiles) * currentMap.tileSize,
                             (float)(i / currentMap.widthTiles) * currentMap.tileSize);


            window.draw(tile);
        }
    }
}


void addPortal(int x, int y, int w, int h, std::string ts, std::string g, std::string s, std::string o, int mw, int tsiz, sf::Vector2f spawn) {
    Portal p;
    p.box.setSize({(float)w * currentMap.tileSize, (float)h * currentMap.tileSize});
    p.box.setPosition((float)x * currentMap.tileSize, (float)y * currentMap.tileSize);
    p.box.setFillColor(sf::Color::Transparent);
    p.ts = ts; p.g = g; p.s = s; p.o = o; p.targetW = mw; p.targetTS = tsiz; p.spawn = spawn;
    portals.push_back(p);
}

sf::View updateCamera(const Player& player, const Map& map) {
    // السر هنا: حجم الـ View بياخد مساحة الماب بالظبط عشان يملأ الشاشة
    float mW = (float)map.widthTiles * map.tileSize;
    float mH = (float)map.heightTiles * map.tileSize;

    // بنعمل View بحجم الماب (لو الماب صغيرة) أو بحجم الشاشة (لو كبيرة)
    // عشان نضمن الـ "Full Look" اللي في الصورة
    sf::View view;
    view.setSize(mW, mH);
    view.setCenter(mW / 2.f, mH / 2.f);

    return view;
}