#include "NPC.h"
#include "Data.h"
#include <cmath>
#include <iostream>
#include "DialogueManager.h"

// ===============================
// NPC ARRAY
// ===============================
NPC allNPCs[MAX_NPCS];
int npcCount = 0;

enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };

// =====================================
// INIT NPCs
// =====================================
void initNPCs() {

    npcCount = 0;

    // =========================
    // MOVING NPC
    // =========================
    NPC student;
    student.name = "Student";

    student.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    student.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    student.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    student.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");

    student.isStatic = false;
    student.speed = 90.f;

    student.waypointsCount = 4;
    student.waypoints[0] = {300, 800};
    student.waypoints[1] = {600, 800};
    student.waypoints[2] = {600, 900};
    student.waypoints[3] = {300, 900};

    student.pos = student.waypoints[0];
    student.currentMap = "outside";

    student.dialogueCount = 0;
    student.dialogues[student.dialogueCount++] = {
        "Hey! I'm touring the campus. It's huge and confusing.", -1};

    student.dialogues[student.dialogueCount++] = {
        "I already got lost like 3 times here.", -1};

    student.dialogues[student.dialogueCount++] = {
        "If you survive this place, you're basically a legend.", -1};

    allNPCs[npcCount++] = student;

    // =========================
    // STATIC NPC
    // =========================
    NPC hadry_static;
    hadry_static.name = "Hadry";

    hadry_static.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    hadry_static.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    hadry_static.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    hadry_static.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");

    hadry_static.isStatic = true;
    hadry_static.pos = {800, 800};
    hadry_static.currentMap = "outside";

    hadry_static.dialogueCount = 0;
    hadry_static.dialogues[hadry_static.dialogueCount++] = {
        "Welcome to the Faculty of Computer Science.", -1};

    hadry_static.dialogues[hadry_static.dialogueCount++] = {
        "Here you will suffer... I mean learn programming.", -1};

    hadry_static.dialogues[hadry_static.dialogueCount++] = {
        "Good luck. You're gonna need it.", -1};

    allNPCs[npcCount++] = hadry_static;

    // =========================
    // INIT SPRITES
    // =========================
    for (int i = 0; i < npcCount; i++) {

        NPC& npc = allNPCs[i];

        npc.sprite.setTexture(npc.walkTextures[SOUTH]);
        npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
        npc.sprite.setOrigin(34.f, 34.f);
        npc.sprite.setScale(1.8f, 1.8f);
        npc.sprite.setPosition(npc.pos);

        npc.currentFrame = 0;
        npc.animTimer = 0.f;
    }
}

// =====================================
// UPDATE NPCs
// =====================================
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos) {

    // 🔥 لو دايلوج شغال وقف كل حاجة
    if (isOpen) return;

    for (int i = 0; i < npcCount; i++) {

        NPC& npc = allNPCs[i];

        if (npc.currentMap != currentMapName)
            continue;

        // =========================
        // MOVING NPC
        // =========================
        if (!npc.isStatic && npc.waypointsCount > 0) {

            sf::Vector2f target = npc.waypoints[npc.currentWaypoint];
            sf::Vector2f moveVec = target - npc.pos;

            float distance = std::sqrt(moveVec.x * moveVec.x + moveVec.y * moveVec.y);

            if (distance > 2.f) {

                sf::Vector2f dir = moveVec / distance;
                sf::Vector2f nextPos = npc.pos + dir * npc.speed * deltaTime;

                sf::FloatRect npcBounds = npc.sprite.getGlobalBounds();
                sf::FloatRect playerBounds(
                    playerPos.x - 10,
                    playerPos.y - 24,
                    20,
                    48
                );

                npcBounds.left = nextPos.x - npcBounds.width / 2;
                npcBounds.top  = nextPos.y - npcBounds.height / 2;

                if (!npcBounds.intersects(playerBounds)) {

                    npc.pos = nextPos;

                    if (std::abs(dir.x) > std::abs(dir.y))
                        npc.sprite.setTexture(dir.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                    else
                        npc.sprite.setTexture(dir.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

                    npc.animTimer += deltaTime;

                    if (npc.animTimer >= 0.1f) {
                        npc.animTimer = 0.f;
                        npc.currentFrame = (npc.currentFrame + 1) % 6;
                    }

                    npc.sprite.setTextureRect(sf::IntRect(npc.currentFrame * 68, 0, 68, 68));
                }
            }
            else {
                npc.currentWaypoint = (npc.currentWaypoint + 1) % npc.waypointsCount;
            }
        }

        // =========================
        // STATIC NPC
        // =========================
        else if (npc.isStatic) {

            float dist =
                std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +
                          std::pow(npc.pos.y - playerPos.y, 2));

            if (dist < 120.f) {

                sf::Vector2f diff = playerPos - npc.pos;

                if (std::abs(diff.x) > std::abs(diff.y))
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
            }
            else {
                npc.sprite.setTexture(npc.walkTextures[SOUTH]);
            }

            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
        }

        npc.sprite.setPosition(npc.pos);
    }
}

// =====================================
// DRAW NPCs
// =====================================
void drawNPCs(sf::RenderWindow& window, std::string currentMapName) {

    for (int i = 0; i < npcCount; i++) {

        NPC& npc = allNPCs[i];

        if (npc.currentMap == currentMapName)
            window.draw(npc.sprite);
    }
}

// =====================================
// INTERACTION
// =====================================
void interactWithNPC(sf::Vector2f playerPos) {

    for (int i = 0; i < npcCount; i++) {

        NPC& npc = allNPCs[i];

        float dist =
            std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +
                      std::pow(npc.pos.y - playerPos.y, 2));

        if (dist < 75.f) {

            sf::Vector2f diff = playerPos - npc.pos;

            if (std::abs(diff.x) > std::abs(diff.y))
                npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
            else
                npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));

            // =========================
            // BUILD DIALOGUE ARRAY
            // =========================
            std::string tempLines[MAX_DIALOGUE_LINES];
            int lineCount = 0;

            for (int j = 0; j < npc.dialogueCount; j++) {
                if (lineCount < MAX_DIALOGUE_LINES)
                    tempLines[lineCount++] = npc.dialogues[j].text;
            }

            // 🔥 FUNCTION CALL (no class)
            if (lineCount > 0)
                startDialogue(npc.name, tempLines, lineCount);

            return;
        }
    }
}