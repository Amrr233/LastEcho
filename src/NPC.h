#ifndef NPC_H
#define NPC_H

#include "Data.h"
#include <SFML/Graphics.hpp>
#include <string>

struct Dialogue {
    std::string text;
    int requiredFlag;
};

struct NPC {
    std::string name;
    sf::Sprite sprite;
    sf::Texture walkTextures[4];
    sf::Vector2f pos;
    std::string currentMap;

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

void initNPCs();
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos);
void drawNPCs(sf::RenderWindow& window, std::string currentMapName);
void interactWithNPC(sf::Vector2f playerPos);

#endif