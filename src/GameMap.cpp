#include "GameMap.h"
#include <fstream>
#include <iostream>

// 1. دالة التحميل: بتملأ الـ struct بالبيانات وبتحفظ اسم الماب للـ NPCs
bool loadMapFromJSON(GameMap& map, const std::string& jsonPath) {

    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open JSON file at " << jsonPath << std::endl;
        return false;
    }

    json mapData;
    try {
        file >> mapData;
    } catch (json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return false;
    }

    // --- [تعديل مهم]: استخراج اسم الماب عشان الـ NPCs يعرفوا يظهروا فين ---
    // لو المسار "assets/maps/outside/outside.json" هياخد كلمة "outside"
    size_t lastSlashPath = jsonPath.find_last_of("/\\");
    size_t lastDot = jsonPath.find_last_of(".");
    if (lastSlashPath != std::string::npos && lastDot != std::string::npos) {
        map.mapName = jsonPath.substr(lastSlashPath + 1, lastDot - lastSlashPath - 1);
    } else {
        map.mapName = "unknown";
    }

    // قراءة الأبعاد الأساسية
    map.width = mapData["width"];
    map.height = mapData["height"];
    map.tileSize = mapData["tileheight"];

    // التعامل مع صورة التايلسيت (Tileset)
    std::string textureName = mapData["tilesets"][0]["image"];
    size_t lastSlash = textureName.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        textureName = textureName.substr(lastSlash + 1);
    }

    // تأكد إن فولدر الصور صح (عدله لو بتستخدم فولدرات مختلفة)
    std::string fullPath = "assets/maps/outside/" + textureName;

    if (!map.tilesetTexture.loadFromFile(fullPath)) {
        std::cerr << "Error: Could not load texture from: " << fullPath << std::endl;
        return false;
    }

    map.tilesetTexture.setSmooth(false);

    // قراءة الليرات (Tiles Only)
    map.layers.clear();
    for (auto& layer : mapData["layers"]) {
        if (layer["type"] == "tilelayer") {
            MapLayer ml;
            ml.name = layer["name"];
            ml.visible = layer["visible"];
            ml.data = layer["data"].get<std::vector<int>>();
            map.layers.push_back(ml);
        }
    }

    return true;
}

// 2. دالة الكاميرا: ضبط الرؤية حسب حجم الماب
sf::View getMapView(const GameMap& map) {
    float mapW = (float)(map.width * map.tileSize);
    float mapH = (float)(map.height * map.tileSize);

    sf::View view;
    view.setSize(mapW, mapH);
    view.setCenter(mapW / 2.0f, mapH / 2.0f);

    return view;
}

// 3. دالة الرسم: رسم كل ليرات البلاط
void drawMap(sf::RenderWindow& window, const GameMap& map) {
    if (map.layers.empty()) return;

    int tilesPerRow = map.tilesetTexture.getSize().x / map.tileSize;
    sf::Sprite tileSprite(map.tilesetTexture);

    for (const auto& layer : map.layers) {
        if (!layer.visible) continue;

        for (int i = 0; i < (int)layer.data.size(); i++) {
            int gid = layer.data[i];
            if (gid == 0) continue;

            int actualID = gid - 1;
            int tx = (actualID % tilesPerRow) * map.tileSize;
            int ty = (actualID / tilesPerRow) * map.tileSize;

            tileSprite.setTextureRect(sf::IntRect(tx, ty, map.tileSize, map.tileSize));

            float px = (float)(i % map.width) * map.tileSize;
            float py = (float)(i / map.width) * map.tileSize;
            tileSprite.setPosition(px, py);

            window.draw(tileSprite);
        }
    }
}