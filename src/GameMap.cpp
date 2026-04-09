#include "GameMap.h"
#include <fstream>
#include <iostream>
#include "player.h"

// 1. تحميل الداتا من JSON
bool loadMapFromJSON(GameMap& map, const std::string& jsonPath) {
    std::ifstream file(jsonPath);
    if (!file.is_open()) {
        std::cerr << "[Error]: Could not open JSON file at " << jsonPath << std::endl;
        return false;
    }

    json mapData;
    try {
        file >> mapData;
    } catch (json::parse_error& e) {
        std::cerr << "[JSON Parse Error]: " << e.what() << std::endl;
        return false;
    }

    map.layers.clear();
    map.portals.clear();

    map.width = mapData["width"];
    map.height = mapData["height"];
    map.tileSize = mapData["tileheight"];

    // تظبيط مسارات الصور
    std::string folderPath = "";
    size_t lastSlashJson = jsonPath.find_last_of("/\\");
    if (lastSlashJson != std::string::npos) {
        folderPath = jsonPath.substr(0, lastSlashJson + 1);
    }

    std::string textureName = mapData["tilesets"][0]["image"];
    size_t lastSlashImg = textureName.find_last_of("/\\");
    if (lastSlashImg != std::string::npos) {
        textureName = textureName.substr(lastSlashImg + 1);
    }

    std::string fullPath = folderPath + textureName;

    if (!map.tilesetTexture.loadFromFile(fullPath)) {
        std::cerr << "[Error]: Texture not found at " << fullPath << std::endl;
        return false;
    }
    map.tilesetTexture.setSmooth(false);

    // قراءة الليرات والبوابات
    for (auto& layer : mapData["layers"]) {
        if (layer["type"] == "objectgroup" && layer["name"] == "Portals") {
            for (auto& obj : layer["objects"]) {
                Portal p;
                p.bounds = sf::FloatRect(obj["x"].get<float>(), obj["y"].get<float>(),
                                        obj["width"].get<float>(), obj["height"].get<float>());

                if (obj.contains("properties")) {
                    for (auto& prop : obj["properties"]) {
                        string key = prop["name"];
                        auto value = prop["value"];

                        if (value.is_null()) continue;

                        if (key == "targetMap") p.targetMap = value.get<std::string>();
                        else if (key == "spawnX") p.spawnPos.x = value.is_number() ? value.get<float>() : std::stof(value.get<string>());
                        else if (key == "spawnY") p.spawnPos.y = value.is_number() ? value.get<float>() : std::stof(value.get<string>());
                    }
                }
                map.portals.push_back(p);
            }
        }

        if (layer["type"] == "tilelayer") {
            MapLayer ml;
            ml.name = layer["name"];
            ml.visible = layer["visible"];
            ml.data = layer["data"].get<std::vector<int>>();
            map.layers.push_back(ml);
        }
    }

    std::cout << "[Success]: Map loaded " << jsonPath << std::endl;
    return true;
}

// 2. الكاميرا
sf::View getMapView(const GameMap& map) {
    float mapW = (float)(map.width * map.tileSize);
    float mapH = (float)(map.height * map.tileSize);
    sf::View view;
    view.setSize(mapW, mapH);
    view.setCenter(mapW / 2.0f, mapH / 2.0f);
    return view;
}

// 3. الرسم
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