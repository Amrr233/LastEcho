#include "NPC.h"      // تعريف كلاس الـ NPC
#include "Data.h"     // بيانات اللعبة
#include <cmath>      // دوال رياضية
#include <iostream>   // للطباعة

// ===============================
// 🔥 استبدال الـ vector بـ array
// ===============================
NPC allNPCs[MAX_NPCS];     // array ثابت
int npcCount = 0;          // عدد الـ NPCs الحالي

enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 };

// =====================================
// INIT (تهيئة الـ NPCs)
// =====================================
void initNPCs() {
    npcCount = 0; // 🔥 بدل clear()

    // =========================
    // NPC متحرك
    // =========================
    NPC student;
    student.name = "Student";
    student.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    student.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    student.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    student.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");
    student.isStatic = false;
    student.speed = 90.f;
    // نقاط الحركة
    student.waypointsCount=4;
    student.waypoints[0]={300, 800};
    student.waypoints[1]={600, 800};
    student.waypoints[2]={600, 900};
    student.waypoints[3]={300, 900};

    student.pos = student.waypoints[0];
    student.currentMap = "outside";
    student.dialogues.push_back({"Hey! I'm touring the campus.", -1});

    // 🔥 إضافة للـ array
    allNPCs[npcCount++] = student;

    // =========================
    // NPC ثابت
    // =========================
    NPC hadry_static;
    hadry_static.name = "hadry";

    hadry_static.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking/walking.south.png");
    hadry_static.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking/walking.north.png");
    hadry_static.walkTextures[WEST].loadFromFile("assets/sprites/player/walking/walking.west.png");
    hadry_static.walkTextures[EAST].loadFromFile("assets/sprites/player/walking/walking.east.png");
    hadry_static.isStatic = true;
    hadry_static.pos = {800, 800};
    hadry_static.currentMap = "outside";
    hadry_static.dialogues.push_back({"Welcome to the Faculty.", -1});
    allNPCs[npcCount++] = hadry_static;
    // إعدادات عامة
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
// UPDATE
// =====================================
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos) {
    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap != currentMapName) continue;
        if (!npc.isStatic && npc.waypointsCount > 0) {
            sf::Vector2f target = npc.waypoints[npc.currentWaypoint];
            sf::Vector2f moveVec = target - npc.pos;
            //moveVec.x -> X axes diff
            //moveVec.y -> Y axes diff
            float distance = std::sqrt(moveVec.x * moveVec.x + moveVec.y * moveVec.y);
            if (distance > 2.0f) {

                sf::Vector2f dir = moveVec / distance;
                // da normalization 3shan law el distance increased or decreased the moving speed stay constant by converting the direction from an actual distance vector to a unit vector

                sf::Vector2f nextPos = npc.pos + dir * npc.speed * deltaTime;
                sf::FloatRect npcBounds = npc.sprite.getGlobalBounds();
                sf::FloatRect playerBounds( //hitbox
                    playerPos.x - 10,
                    playerPos.y - 24,
                    20,
                    48
                );

                npcBounds.left = nextPos.x - npcBounds.width / 2;
                npcBounds.top  = nextPos.y - npcBounds.height / 2;

                if (npcBounds.intersects(playerBounds)) {

                    npc.animTimer = 0.f;
                    npc.currentFrame = 0;

                    sf::Vector2f diff = playerPos - npc.pos;
                    if (std::abs(diff.x) > std::abs(diff.y)) {
                        npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                    } else {
                        npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                    }

                    npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
                }
                else {//ta7ded el dirc if its moving no collision
                    npc.pos = nextPos;

                    if (std::abs(dir.x) > std::abs(dir.y)) {
                        npc.sprite.setTexture(dir.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                    } else {
                        npc.sprite.setTexture(dir.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                    }

                    npc.animTimer += deltaTime;

                    if (npc.animTimer >= 0.1f) {
                        npc.animTimer = 0.f;
                        npc.currentFrame = (npc.currentFrame + 1) % 6;
                    }
                    npc.sprite.setTextureRect(sf::IntRect(npc.currentFrame * 68, 0, 68, 68));
                }

            } else {npc.currentWaypoint = (npc.currentWaypoint + 1) % npc.waypointsCount; //لو وصل للبوينت دي يروح للبعدها
            }
        }
        else if (npc.isStatic) {

            float distToPlayer = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) +std::pow(npc.pos.y - playerPos.y, 2));
            if (distToPlayer < 120.0f) {
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y)) {
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                } else {
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                }
            } else {
                npc.sprite.setTexture(npc.walkTextures[SOUTH]);
            }
            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
        }
        npc.sprite.setPosition(npc.pos);
    }
}
// DRAW
void drawNPCs(sf::RenderWindow& window, std::string currentMapName) {
    for (int i = 0; i < npcCount; i++) {
        const NPC& npc = allNPCs[i];
        if (npc.currentMap == currentMapName) {window.draw(npc.sprite);}
    }
}
// INTERACTION
void interactWithNPC(sf::Vector2f playerPos) {

    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];

        float dist = std::sqrt(
            std::pow(npc.pos.x - playerPos.x, 2) +
            std::pow(npc.pos.y - playerPos.y, 2)
        );

        if (dist < 75.0f) {

            sf::Vector2f diff = playerPos - npc.pos;
            if (std::abs(diff.x) > std::abs(diff.y)) {
                npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
            } else {
                npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
            }

            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));

            std::cout << npc.name << ": "
                      << npc.dialogues[0].text << std::endl;

            return;
        }
    }
}