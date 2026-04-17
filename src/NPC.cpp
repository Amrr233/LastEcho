#include "NPC.h"
#include "Data.h"
#include <cmath>
#include <iostream>
#include "DialogueManager.h"
#include "world.h"
#include "Phase.h"

NPC allNPCs[MAX_NPCS];
int npcCount = 0;

enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };

// =====================================
// INIT
// =====================================
void initNPCs(World& world) {
    npcCount = 0;

    // 1. الصاحب (Friend_NPC)
    NPC friendNPC;
    friendNPC.name = "Friend_NPC";
    friendNPC.walkTextures[SOUTH].loadFromFile("assets/sprites/npcs/fares/walking-south.png");
    friendNPC.walkTextures[NORTH].loadFromFile("assets/sprites/npcs/fares/walking-north.png");
    friendNPC.walkTextures[WEST].loadFromFile("assets/sprites/npcs/fares/walking-west.png");
    friendNPC.walkTextures[EAST].loadFromFile("assets/sprites/npcs/fares/walking-east.png");

    friendNPC.isStatic = false;
    friendNPC.speed = 90.f;
    friendNPC.currentMap = "outside";

    // --- إعدادات الانتظار ---
    friendNPC.waitTimer = 0.f;     // العداد بيبدأ من صفر
    friendNPC.waitTime = 6.5f;    // هينتظر   ثواني
    friendNPC.isWaiting = true;   // هيبدأ اللعبة وهو مستني

    friendNPC.waypointsCount = 4;
    friendNPC.waypoints[0] = {300, 800};
    friendNPC.waypoints[1] = {600, 800};
    friendNPC.waypoints[2] = {600, 900};
    friendNPC.waypoints[3] = {300, 900};
    friendNPC.pos = friendNPC.waypoints[0];
    friendNPC.currentWaypoint = 0;

    allNPCs[npcCount++] = friendNPC;

    // 2. الأمن (Security_Guard)
    NPC guard;
    guard.name = "Security_Guard";
    guard.walkTextures[SOUTH].loadFromFile("assets/sprites/npcs/security/security.png");
    for(int i=1; i<4; i++) guard.walkTextures[i] = guard.walkTextures[SOUTH];

    guard.isStatic = true;
    guard.isWaiting = false; // الأمن مش محتاج ينتظر لأنه ثابت أصلاً
    guard.pos = {500, 600};
    guard.currentMap = "outside";
    allNPCs[npcCount++] = guard;

    // تهيئة السبرايتس
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
    if (dialogueSystem.isDialogueActive()) return;

    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap != currentMapName) continue;

        float distToPlayer = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +
                                        std::pow(npc.pos.y - playerPos.y, 2));

        // --- منطق الـ Moving NPC ---
        if (!npc.isStatic && npc.waypointsCount > 0) {

            // 1. فحص حالة الانتظار أولاً
            if (npc.isWaiting) {
                npc.waitTimer += deltaTime;

                // لو اللاعب قرب وهو لسه مستني، يلف يبص عليه بس ميمشيش
                if (distToPlayer < 80.0f) {
                    sf::Vector2f diff = playerPos - npc.pos;
                    if (std::abs(diff.x) > std::abs(diff.y))
                        npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                    else
                        npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                } else {
                    // الوضع الطبيعي وهو واقف (فريم الثبات)
                    npc.sprite.setTexture(npc.walkTextures[SOUTH]);
                }
                npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
                npc.sprite.setPosition(npc.pos);

                // لو الوقت خلص، ابدأ الحركة
                if (npc.waitTimer >= npc.waitTime) {
                    npc.isWaiting = false;
                }
                continue; // تخطي باقي كود الحركة طول ما هو مستني
            }

            // 2. كمل كود الحركة الأصلي بتاعك
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
                // ملحوظة: لو عايزه يستنى 10 ثواني عند كل نقطة يوصلها، ضيف السطرين دول هنا:
                npc.isWaiting = true;
                npc.waitTimer = 0.f;
            }
        }
        // --- منطق الـ Static NPC ---
        else if (npc.isStatic) {
            if (distToPlayer < 120.0f) {
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y))
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
            } else {
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

// =====================================
// INTERACTION
// =====================================
std::string getNearbyNPCName(sf::Vector2f playerPos, std::string currentMap) {
    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap == currentMap) {
            float dist = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) + std::pow(npc.pos.y - playerPos.y, 2));
            if (dist < 75.0f) {
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y))
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

                npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
                return npc.name;
            }
        }
    }
    return "";
}