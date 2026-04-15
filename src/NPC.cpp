#include "NPC.h"
#include "Data.h"
#include <cmath>
#include <iostream>
#include "DialogueManager.h"

NPC allNPCs[MAX_NPCS];
int npcCount = 0;

enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };

void initNPCs() {
    npcCount = 0;

    // NPC 1: Student
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

    // 🔥 Expanded dialogues for testing E + wrapping + pages
    student.dialogues[student.dialogueCount++] = {"Hey! I'm touring the campus.", -1};
    student.dialogues[student.dialogueCount++] = {"This place is really huge and full of secrets you can explore.", -1};
    student.dialogues[student.dialogueCount++] = {"If you keep walking around, you might find hidden areas or NPCs you didn't expect.", -1};
    student.dialogues[student.dialogueCount++] = {"Also, the dialogue system supports multiple pages like this one, try pressing E!", -1};
    student.dialogues[student.dialogueCount++] = {"This is another long sentence just to test the text wrapping inside the dialogue box width limit system.", -1};
    student.dialogues[student.dialogueCount++] = {"Final line: everything is working fine if you can read this smoothly without overflow.", -1};


    allNPCs[npcCount++] = student;

    // NPC 2: Hadry
    NPC hadry_static;
    hadry_static.name = "hadry";
    hadry_static.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    hadry_static.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    hadry_static.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    hadry_static.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");
    hadry_static.isStatic = true;
    hadry_static.pos = {800, 800};
    hadry_static.currentMap = "outside";
    hadry_static.dialogueCount = 0;

    // 🔥 Expanded static NPC dialogue too
    hadry_static.dialogues[hadry_static.dialogueCount++] = {"Welcome to the Faculty.", -1};
    hadry_static.dialogues[hadry_static.dialogueCount++] = {"You can interact with multiple NPCs across the map.", -1};
    hadry_static.dialogues[hadry_static.dialogueCount++] = {"Try pressing E near different characters to test the system.", -1};
    hadry_static.dialogues[hadry_static.dialogueCount++] = {"This system also supports multi-line wrapping and pagination.", -1};
    hadry_static.dialogues[hadry_static.dialogueCount++] = {"If this text breaks nicely, your wrapText function is working correctly.", -1};

    allNPCs[npcCount++] = hadry_static;

    // setup sprites
    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        npc.sprite.setTexture(npc.walkTextures[SOUTH]);
        npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
        npc.sprite.setOrigin(34.f, 34.f);
        npc.sprite.setScale(1.8f, 1.8f);
        npc.sprite.setPosition(npc.pos);
    }
}

void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos) {
    if (isDialogueActive()) return;

    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap != currentMapName) continue;

        float distToPlayer = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +
                                        std::pow(npc.pos.y - playerPos.y, 2));

        if (!npc.isStatic && npc.waypointsCount > 0) {

            sf::Vector2f target = npc.waypoints[npc.currentWaypoint];
            sf::Vector2f moveVec = target - npc.pos;
            float distance = std::sqrt(moveVec.x * moveVec.x + moveVec.y * moveVec.y);

            if (distToPlayer < 80.0f) {
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y))
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

                npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
            }
            else if (distance > 2.0f) {
                sf::Vector2f dir = moveVec / distance;
                npc.pos += dir * npc.speed * deltaTime;

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
            else {
                npc.currentWaypoint = (npc.currentWaypoint + 1) % npc.waypointsCount;
            }
        }
        else if (npc.isStatic) {

            if (distToPlayer < 120.0f) {
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

void drawNPCs(sf::RenderWindow& window, std::string currentMapName) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].currentMap == currentMapName)
            window.draw(allNPCs[i].sprite);
    }
}

void interactWithNPC(sf::Vector2f playerPos) {
    for (int i = 0; i < npcCount; i++) {

        NPC& npc = allNPCs[i];

        float dist = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +
                                std::pow(npc.pos.y - playerPos.y, 2));

        if (dist < 75.0f) {

            sf::Vector2f diff = playerPos - npc.pos;

            if (std::abs(diff.x) > std::abs(diff.y))
                npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
            else
                npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));

            std::string tempLines[MAX_DIALOGUE_LINES];
            int lineCount = 0;

            for (int j = 0; j < npc.dialogueCount; j++) {
                tempLines[lineCount++] = npc.dialogues[j].text;
            }

            if (lineCount > 0)
                startDialogue(npc.name, tempLines, lineCount);

            return;
        }
    }
}