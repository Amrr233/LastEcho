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
// INIT (دمج تحميل كل الصور والبيانات)
// =====================================
void initNPCs(World& world) {
    npcCount = 0;

    // 1. الصاحب (Friend_NPC) - ده بيتحرك
    NPC friendNPC;
    friendNPC.name = "Friend_NPC";
    friendNPC.walkTextures[SOUTH].loadFromFile("assets/sprites/npcs/fares/walking-south.png");
    friendNPC.walkTextures[NORTH].loadFromFile("assets/sprites/npcs/fares/walking-north.png");
    friendNPC.walkTextures[WEST].loadFromFile("assets/sprites/npcs/fares/walking-west.png");
    friendNPC.walkTextures[EAST].loadFromFile("assets/sprites/npcs/fares/walking-east.png");

    friendNPC.isStatic = false; // خليه يتحرك
    friendNPC.speed = 90.f;
    friendNPC.currentMap = "outside";

    // إعداد الـ Waypoints (نفس اللي كان في كودك القديم)
    friendNPC.waypointsCount = 4;
    friendNPC.waypoints[0] = {300, 800};
    friendNPC.waypoints[1] = {600, 800};
    friendNPC.waypoints[2] = {600, 900};
    friendNPC.waypoints[3] = {300, 900};
    friendNPC.pos = friendNPC.waypoints[0];
    friendNPC.currentWaypoint = 0;

    allNPCs[npcCount++] = friendNPC;

    // 2. الأمن (Security_Guard) - ده ثابت
    NPC guard;
    guard.name = "Security_Guard";
    guard.walkTextures[SOUTH].loadFromFile("assets/sprites/npcs/security/security.png");
    for(int i=1; i<4; i++) guard.walkTextures[i] = guard.walkTextures[SOUTH]; // تأمين المربع الأبيض

    guard.isStatic = true;
    guard.pos = {500, 600};
    guard.currentMap = "outside";
    allNPCs[npcCount++] = guard;

    // تهيئة السبرايتس والتايمر
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
// UPDATE (منطق الحركة والأنيميشن الأصلي بتاعك)
// =====================================
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos) {
    if (dialogueSystem.isDialogueActive()) return;

    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap != currentMapName) continue;

        float distToPlayer = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +
                                        std::pow(npc.pos.y - playerPos.y, 2));

        // --- منطق الـ Moving NPC (الـ Waypoints) ---
        if (!npc.isStatic && npc.waypointsCount > 0) {
            sf::Vector2f target = npc.waypoints[npc.currentWaypoint];
            sf::Vector2f moveVec = target - npc.pos;
            float distance = std::sqrt(moveVec.x * moveVec.x + moveVec.y * moveVec.y);

            if (distToPlayer < 80.0f) { // لو اللاعب قريب، بطل حركة وبص عليه
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y))
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

                npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
            }
            else if (distance > 2.0f) { // كمل مشي للـ Waypoint
                sf::Vector2f dir = moveVec / distance;
                npc.pos += dir * npc.speed * deltaTime;

                // تحديث الاتجاه بناءً على الحركة
                if (std::abs(dir.x) > std::abs(dir.y))
                    npc.sprite.setTexture(dir.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(dir.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

                // أنميشن المشي (الـ 6 فريمات بتوعك)
                npc.animTimer += deltaTime;
                if (npc.animTimer >= 0.1f) {
                    npc.animTimer = 0.f;
                    npc.currentFrame = (npc.currentFrame + 1) % 6;
                }
                npc.sprite.setTextureRect(sf::IntRect(npc.currentFrame * 68, 0, 68, 68));
            }
            else { // وصل للـ Waypoint، انقل على اللي بعده
                npc.currentWaypoint = (npc.currentWaypoint + 1) % npc.waypointsCount;
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
// =====================================
// DRAW
// =====================================
void drawNPCs(sf::RenderWindow& window, std::string currentMapName, int currentPhase) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].currentMap == currentMapName) {
            // شيلنا شرط الاختفاء حالياً عشان ميعملش مشاكل لحد ما نتأكد إن السيستم شغال
            window.draw(allNPCs[i].sprite);
        }
    }
}

// =====================================
// INTERACTION (الربط مع الـ PhaseSystem)
// =====================================
std::string getNearbyNPCName(sf::Vector2f playerPos, std::string currentMap) {
    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap == currentMap) {
            float dist = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) + std::pow(npc.pos.y - playerPos.y, 2));
            if (dist < 75.0f) {
                // لف الـ NPC ناحية اللاعب (منطقك)
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