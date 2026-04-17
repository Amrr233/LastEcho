#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <SFML/Graphics.hpp>
#include <json.hpp>
#include <vector>
#include <string>
using namespace std;
using namespace sf;

using json = nlohmann::json;

// 1. هيكل بيانات الطبقة الواحدة


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

// 2. هيكل بيانات الماب بالكامل
struct GameMap {
    int width = 0;             // عدد التايلات عرضاً
    int height = 0;            // عدد التايلات طولاً
    int tileSize = 0;          // حجم التايلة بالبكسل
    sf::Texture tilesetTexture;      // الصورة المستخدمة
    vector<MapLayer> layers;
    vector<Portal> portals;// ليرات الماب (Ground, Staircase, etc.)
};

// 3. الدوال المسؤولة عن العمليات (بتاخد الماب كـ reference)

// تحميل البيانات من الـ JSON ونقلها للـ struct
bool loadMapFromJSON(GameMap& map, const std::string& jsonPath);

// رسم الماب بالكامل
void drawMap(sf::RenderWindow& window, const GameMap& map);

// حساب الكاميرا المتموطة (Stretch) بناءً على بيانات الـ struct
View updateMapView(View& currentView, const GameMap& map, Vector2f playerPos, float deltaTime);

// دوال مساعدة للحسابات
inline int getMapWidthPixels(const GameMap& map) { return map.width * map.tileSize; }
inline int getMapHeightPixels(const GameMap& map) { return map.height * map.tileSize; }

// Check if a position is walkable
bool mapIsWalkable(const GameMap& map, float x, float y);

// Check collision with player bounds
bool mapCheckCollision(const GameMap& map, sf::FloatRect playerBounds, const std::string& mapName);

#endif