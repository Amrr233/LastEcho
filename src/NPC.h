#ifndef NPC_H
#define NPC_H
#include "Data.h"
#include <SFML/Graphics.hpp>
#include <string>

struct Dialogue {
    std::string text;
    int requiredFlag; // لو -1 يبقى متاح للكل، لو رقم يبقى لازم الـ flag ده يكون true
};

struct NPC {
    std::string name;
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture walkTextures[4];

    sf::Vector2f pos;
    std::string currentMap; // الماب اللي الـ NPC موجود فيها حالياً

    // الحركة
    bool isStatic;         // هل واقف مكانه ولا بيمشي؟
    sf::Vector2f waypoints[MAX_WAYPOINTS];
    int waypointsCount;
    int currentWaypoint = 0;
    float speed = 50.0f;

    // 🔥 الحوارات كـ Array ثابت
    Dialogue dialogues[MAX_DIALOGUE_LINES];
    int dialogueCount = 0; // عداد يدوي بدل الـ vector
    int currentDialogueIdx = 0;

    // أنميشن خاص بكل NPC
    float animTimer = 0.f;
    int currentFrame = 0;
};

// الدوال الأساسية
void initNPCs();
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos);
void drawNPCs(sf::RenderWindow& window, std::string currentMapName);
void interactWithNPC(sf::Vector2f playerPos);

#endif