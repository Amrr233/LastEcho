#ifndef NPC_H
#define NPC_H
#include "Data.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "world.h"

struct Dialogue {
    std::string text;
    int requiredFlag; // لو -1 يبقى متاح للكل، لو رقم يبقى لازم الـ flag ده يكون true
};

struct NPC {
    std::string name;
    Sprite sprite;
    Texture texture;
    Texture walkTextures[4];
    float waitTimer = 0.f;      // العداد الحالي
    float waitTime = 10.f;     // الوقت المطلوب للانتظار (10 ثواني)
    bool isWaiting = true;     // هل هو حالياً في حالة انتظار؟
    Vector2f pos;
    std::string currentMap; // الماب اللي الـ NPC موجود فيها حالياً

    // الحركة
    bool isStatic;         // هل واقف مكانه ولا بيمشي؟
    // std::vector<sf::Vector2f> waypoints; // نقط الحركة لو مش static
    sf::Vector2f waypoints[MAX_WAYPOINTS];
    int waypointsCount;
    int currentWaypoint = 0;
    float speed = 50.0f;

    // الحوارات
    std::vector<Dialogue> dialogues;
    int currentDialogueIdx = 0;
    // أنميشن خاص بكل NPC
    float animTimer = 0.f;
    int currentFrame = 0;
};

//flags in data.h

// الدوال الأساسية
void initNPCs(World& world);
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos);
void drawNPCs(sf::RenderWindow& window, std::string currentMapName, int currentPhase);
void interactWithNPC(sf::Vector2f playerPos);
std::string getNearbyNPCName(sf::Vector2f playerPos, std::string currentMap);

#endif