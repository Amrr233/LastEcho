#ifndef NPC_H
#define NPC_H

#include "Data.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "world.h"

struct Dialogue {
    std::string text;
    int requiredFlag;
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

    bool isStatic;
    sf::Vector2f waypoints[MAX_WAYPOINTS];
    int waypointsCount = 0;
    int currentWaypoint = 0;
    float speed = 50.0f;

    Dialogue dialogues[MAX_DIALOGUE_LINES];
    int dialogueCount = 0;

    float animTimer = 0.f;
    int currentFrame = 0;
};

//flags in data.h

// الدوال الأساسية
void initNPCs(World& world);
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos);
void drawNPCs(sf::RenderWindow& window, std::string currentMapName, int currentPhase);
void interactWithNPC(sf::Vector2f playerPos);
bool checkNPCCollision(sf::FloatRect playerBounds, std::string currentMap);
std::string getNearbyNPCName(sf::Vector2f playerPos, std::string currentMap);

#endif