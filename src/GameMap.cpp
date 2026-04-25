#include "GameMap.h"
#include <fstream>
#include <iostream>
#include "player.h"

// 1. دالة التحميل: بتملأ الـ struct بالبيانات وبتحفظ اسم الماب للـ NPCs
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

    // تنظيف البيانات القديمة
    map.layers.clear();
    map.portals.clear();
    map.tileProperties.clear();

    // التأكد من أن الملف عبارة عن "خريطة" وليس مجرد "تايلست"
    if (!mapData.contains("width") || !mapData.contains("tileheight")) {
        std::cerr << "[Error]: " << jsonPath << " is not a valid Tiled Map (missing width/height)!" << std::endl;
        return false;
    }

    map.width = mapData["width"];
    map.height = mapData["height"];
    map.tileSize = mapData["tileheight"];

    // 1. قراءة الـ Tile Properties (الجزء اللي بيخلي الحيطان ناشفة)
    if (mapData.contains("tilesets")) {
        for (const auto& tileset : mapData["tilesets"]) {
            if (!tileset.contains("firstgid")) continue;
            int firstGid = tileset["firstgid"];

            if (tileset.contains("tiles")) {
                for (const auto& tileJson : tileset["tiles"]) {
                    if (!tileJson.contains("id")) continue;
                    int localId = tileJson["id"];
                    int globalId = firstGid + localId;

                    if (tileJson.contains("properties")) {
                        for (const auto& prop : tileJson["properties"]) {
                            if (!prop.contains("name") || !prop.contains("value")) continue;

                            std::string pName = prop["name"];
                            if (pName == "solid") {
                                bool isSolid = false;
                                if (prop["value"].is_boolean())
                                    isSolid = prop["value"].get<bool>();
                                else if (prop["value"].is_string())
                                    isSolid = (prop["value"].get<std::string>() == "true");

                                map.tileProperties[globalId][pName] = isSolid ? "true" : "false";
                            }
                        }
                    }
                }
            }
        }
    }

    // 2. تظبيط مسارات الصور
    std::string folderPath = "";
    size_t lastSlashJson = jsonPath.find_last_of("/\\");
    if (lastSlashJson != std::string::npos) {
        folderPath = jsonPath.substr(0, lastSlashJson + 1);
    }

    if (!mapData["tilesets"].empty()) {
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
    }
    map.tilesetTexture.setSmooth(false);

    // 3. قراءة الليرات والبوابات
    for (auto& layer : mapData["layers"]) {
        // قراءة البوابات
        if (layer["type"] == "objectgroup" && layer["name"] == "Portals") {
            for (auto& obj : layer["objects"]) {
                Portal p;
                p.spawnPos = sf::Vector2f(0.0f, 0.0f);
                p.bounds = sf::FloatRect(obj["x"].get<float>(), obj["y"].get<float>(),
                                        obj["width"].get<float>(), obj["height"].get<float>());

                if (obj.contains("properties")) {
                    for (auto& prop : obj["properties"]) {
                        if (!prop.contains("name") || prop["value"].is_null()) continue;
                        std::string key = prop["name"];
                        auto value = prop["value"];

                        if (key == "targetMap") p.targetMap = value.get<std::string>();
                        else if (key == "spawnX") p.spawnPos.x = value.is_number() ? value.get<float>() : std::stof(value.get<std::string>());
                        else if (key == "spawnY") p.spawnPos.y = value.is_number() ? value.get<float>() : std::stof(value.get<std::string>());
                    }
                }
                map.portals.push_back(p);
            }
        }

        // قراءة ليرات التايلات
        if (layer["type"] == "tilelayer") {
            MapLayer ml;
            ml.name = layer["name"];
            ml.visible = layer.value("visible", true);
            if (layer.contains("data")) {
                ml.data = layer["data"].get<std::vector<int>>();
            }
            map.layers.push_back(ml);
        }
    }

    std::cout << "[Success]: Map loaded " << jsonPath << std::endl;
    std::cout << "DEBUG: Map loaded from " << jsonPath << " | Properties count: " << map.tileProperties.size() << std::endl;
    return true;
}

//الكاميرا بتجري ورا اللاعب حرفيا

View updateMapView(sf::View& currentView, const GameMap& map, sf::Vector2f playerPos, float deltaTime) { //

    //المكان العايزك تشوفه هو مكان اللاعب دلوقتي
    Vector2f targetPos = playerPos;

    // 2. سرعة جري الكاميرا
    float followSpeed = 5.f;

    // 3. معادلة اسمها lerp بتخلي التتبع بتاع الكاميرا smooth اكتر

    sf::Vector2f currentCenter = currentView.getCenter();
    sf::Vector2f newCenter;

    newCenter.x = currentCenter.x + (targetPos.x - currentCenter.x) *followSpeed * deltaTime;
    newCenter.y = currentCenter.y + (targetPos.y - currentCenter.y) *followSpeed * deltaTime;

    // منع الكاميرا من الخروج بره حدود الماب

    float mapW = (float)(map.width * map.tileSize);
    float mapH = (float)(map.height * map.tileSize);
    Vector2f viewSize = currentView.getSize();



    if (mapW <= viewSize.x) {
        newCenter.x = mapW / 2.0f;
    }
    else {
        // لو الماب أكبر، اعمل Clamp عادي عشان متخرجش بره
        if (newCenter.x - viewSize.x / 2.0f < 0) newCenter.x = viewSize.x / 2.0f;
        if (newCenter.x + viewSize.x / 2.0f > mapW) newCenter.x = mapW - viewSize.x / 2.0f;
    }

    // بالطول (Y): نفس المنطق
    if (mapH <= viewSize.y) {
        newCenter.y = mapH / 2.0f;
    }
    else {
        if (newCenter.y - viewSize.y / 2.0f < 0) newCenter.y = viewSize.y / 2.0f;
        if (newCenter.y + viewSize.y / 2.0f > mapH) newCenter.y = mapH - viewSize.y / 2.0f;
    }

    currentView.setCenter(newCenter);
    return currentView;// برحع الفيو الهيظهر في الاخر للاعب
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

bool mapIsWalkable(const GameMap& map, float x, float y, const std::string& mapName) {
    int tileX = (int)x / map.tileSize;
    int tileY = (int)y / map.tileSize;

    if (tileX < 0 || tileX >= map.width || tileY < 0 || tileY >= map.height) return false;
    int index = tileY * map.width + tileX;

    // --- ماب الـ outside لوحدها ---
    if (mapName == "outside") {
        for (const auto& layer : map.layers) {
            if (index >= 0 && index < (int)layer.data.size()) {
                if (layer.data[index] != 0 && layer.name != "Ground") return false;
            }
        }
        return true;
    }

    // --- ماب الـ lobby لوحدها ---
    if (mapName == "lobby") {
        for (const auto& layer : map.layers) {
            if (index >= 0 && index < (int)layer.data.size()) {
                if (layer.data[index] != 0 && layer.name != "ground") return false;
            }
        }
        return true;
    }

    // --- ماب الـ leftPassage لوحدها (نظام الـ solid property) ---
    if (mapName == "leftPassage" || mapName == "rightPassage"|| mapName == "clinic"|| mapName == "sclab"|| mapName == "datacenter"|| mapName == "class7"||mapName =="secur"||mapName == "class8"||mapName =="hallAfter"||mapName =="wcw"||mapName == "wcm1"||mapName=="wcm2"||mapName == "vertPassage"||mapName=="connHall") {
     for (const auto& layer : map.layers) {
        int gid = layer.data[index];
    //     // هل الرقم ده متسجل له خواص؟
        if (map.tileProperties.count(gid)) {
            // بنقرأ قيمة solid
            if (map.tileProperties.at(gid).count("solid")) {
                std::string isSolid = map.tileProperties.at(gid).at("solid");

                // ركز هنا: مش هنوقف اللاعب إلا لو لقى كلمة "true" صريحة
                    return false;
            }
        }
    }
    // لو خلص كل الطبقات ومالقاش ولا واحدة "true" يبقى يمشي عادي
    return true;
}
    return true;
}

bool mapCheckCollision(const GameMap& map, sf::FloatRect playerBounds, const std::string& mapName) {

    // حسابات الهيت بوكس (زي ما هي)
    float hbW = playerBounds.width * 0.6f - 42.f;
    float hbH = playerBounds.height * 0.3f - 20.f;
    float hbLeft = playerBounds.left + (playerBounds.width - hbW) / 2.f + 2.f;
    float hbTop = playerBounds.top + (playerBounds.height - hbH) - 23.f;

    // --- ماب الاوتسايد ---
    if (mapName == "outside") {
        if (!mapIsWalkable(map, hbLeft, hbTop, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft + hbW, hbTop, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft, hbTop + hbH, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft + hbW, hbTop + hbH, mapName)) return true;
        return false;
    }

    // --- ماب اللوبي ---
    if (mapName == "lobby") {
        if (!mapIsWalkable(map, hbLeft, hbTop, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft + hbW, hbTop, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft, hbTop + hbH, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft + hbW, hbTop + hbH, mapName)) return true;
        return false;
    }

    // --- ضيف ماب الـ leftPassage هنا عشان الكود يشوفها ---
    if (mapName == "leftPassage" || mapName == "rightPassage"|| mapName == "clinic"||mapName == "sclab"||mapName == "datacenter"||mapName == "class7"||mapName =="secur"||mapName == "class8"||mapName =="hallAfter"||mapName =="wcw" ||mapName == "wcm1"||mapName=="wcm2"||mapName =="vertPassage"||mapName=="connHall") {
        if (!mapIsWalkable(map, hbLeft, hbTop, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft + hbW, hbTop, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft, hbTop + hbH, mapName)) return true;
        if (!mapIsWalkable(map, hbLeft + hbW, hbTop + hbH, mapName)) return true;
        return false;
    }

    return false;
}

bool mapSwapTileset(GameMap& map, const std::string& newTexturePath) {
    sf::Texture newTex;
    if (newTex.loadFromFile(newTexturePath)) {
        map.tilesetTexture = newTex; // استبدال الصورة القديمة بالجديدة
        map.tilesetTexture.setSmooth(false); // عشان بكسل أرت يفضل حاد
        return true;
    }
    std::cerr << "[Error]: Could not find cursed texture at " << newTexturePath << std::endl;
    return false;
}
