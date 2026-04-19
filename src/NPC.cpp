#include "NPC.h"
#include "Data.h"
#include <cmath>
#include <iostream>
#include "DialogueManager.h"
#include "world.h"
#include "Phase.h"
#include "player.h"
NPC allNPCs[MAX_NPCS];
int npcCount = 0;
static sf::RectangleShape npcHitboxDebug;
// =====================================
// INIT
// =====================================
sf::FloatRect getNPCHitbox(const NPC& npc) {
    sf::FloatRect bounds = npc.sprite.getGlobalBounds();

    float w = bounds.width * 0.3f;
    float h = bounds.height * 0.6f;

    return sf::FloatRect(
        (bounds.left + (bounds.width - w) / 2.f)-5.f,
        bounds.top + bounds.height - h,
        w,
        h
    );
}
void initNPCs(World& world) {
    npcCount = 0;

    // 1. الصاحب (Friend_NPC)
    NPC friendNPC;
    friendNPC.name = "Friend_NPC";
    friendNPC.walkTextures[SOUTH].loadFromFile("assets/sprites/npcs/fares/walking-south.png");
    friendNPC.walkTextures[NORTH].loadFromFile("assets/sprites/npcs/fares/walking-north.png");
    friendNPC.walkTextures[WEST].loadFromFile("assets/sprites/npcs/fares/walking-west.png");
    friendNPC.walkTextures[EAST].loadFromFile("assets/sprites/npcs/fares/walking-east.png");
    friendNPC.avatarPath = "assets/sprites/NPCS/fares/fares_avatar.png";
friendNPC.avatarTexture.loadFromFile(friendNPC.avatarPath);
friendNPC.avatarSprite.setTexture(friendNPC.avatarTexture);
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
    friendNPC.avatarPath = "assets/sprites/NPCS/fares/fares_avatar.png";

    if (!friendNPC.avatarTexture.loadFromFile(friendNPC.avatarPath)) {
        std::cout << "FAILED TO LOAD NPC AVATAR\n";
    }

    friendNPC.avatarSprite.setTexture(friendNPC.avatarTexture, true);
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


    NPC amr;
    amr.name = "amr";
    amr.walkTextures[SOUTH].loadFromFile("assets/sprites/npcs/amr/walking-south.png");
    amr.walkTextures[NORTH].loadFromFile("assets/sprites/npcs/amr/walking-north.png");
    amr.walkTextures[WEST].loadFromFile("assets/sprites/npcs/amr/walking-west.png");
    amr.walkTextures[EAST].loadFromFile("assets/sprites/npcs/amr/walking-east.png");
    amr.avatarPath = "assets/sprites/npcs/amr/amr_avatar.png";
    amr.avatarTexture.loadFromFile(amr.avatarPath);
    amr.avatarSprite.setTexture(amr.avatarTexture);
    amr.isStatic = true;
    amr.speed = 90.f;
    amr.currentMap = "hallAfter";

    // --- إعدادات الانتظار ---
    amr.waitTimer = 0.f;     // العداد بيبدأ من صفر
    amr.waitTime = 6.5f;    // هينتظر   ثواني
    amr.isWaiting = true;   // هيبدأ اللعبة وهو مستني

    amr.waypointsCount = 1;
    amr.waypoints[0] = {400, 400};
    amr.avatarPath = "assets/sprites/npcs/amr/walking-east.png";

    if (!friendNPC.avatarTexture.loadFromFile(amr.avatarPath)) {
        std::cout << "FAILED TO LOAD NPC AVATAR\n";
    }

    amr.avatarSprite.setTexture(amr.avatarTexture, true);
    amr.pos = amr.waypoints[0];
    amr.currentWaypoint = 0;

    allNPCs[npcCount++] = amr;

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
    if (isDialogueActive()) return;

    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];
        if (npc.currentMap != currentMapName) continue;

        float distToPlayer = std::sqrt(
            std::pow(npc.pos.x - playerPos.x, 2) +
            std::pow(npc.pos.y - playerPos.y, 2)
        );

        // ===== PLAYER HITBOX (FIXED) =====
        float px = playerPos.x;
        float py = playerPos.y;

        sf::FloatRect playerBox(
            px - 15.f,
            py + 10.f,
            30.f,
            20.f
        );

        // ===== MOVING NPC =====
        if (!npc.isStatic && npc.waypointsCount > 0) {

            if (npc.isWaiting) {
                npc.waitTimer += deltaTime;

                if (distToPlayer < 100.0f) {
                    sf::Vector2f diff = playerPos - npc.pos;
                    if (std::abs(diff.x) > std::abs(diff.y))
                        npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                    else
                        npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                } else {
                    npc.sprite.setTexture(npc.walkTextures[SOUTH]);
                }

                npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
                npc.sprite.setPosition(npc.pos);

                if (npc.waitTimer >= npc.waitTime) {
                    npc.isWaiting = false;
                }

                continue;
            }

            sf::Vector2f target = npc.waypoints[npc.currentWaypoint];
            sf::Vector2f moveVec = target - npc.pos;
            float distance = std::sqrt(moveVec.x * moveVec.x + moveVec.y * moveVec.y);

            if (distToPlayer < 100.0f) {
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y))
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                else
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);

                npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
            }
            else if (distance > 2.0f) {

                sf::Vector2f dir = moveVec / distance;

                // 🔥 IMPORTANT PART (ANTI-FREEZE)
                sf::Vector2f oldPos = npc.pos;
                npc.pos += dir * npc.speed * deltaTime;

                npc.sprite.setPosition(npc.pos);
                sf::FloatRect npcBox = getNPCHitbox(npc);

                // لو خبط في اللاعب → رجع مكانه
                if (npcBox.intersects(playerBox)) {
                    npc.pos = oldPos;
                }

                // direction sprite
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
                npc.isWaiting = true;
                npc.waitTimer = 0.f;
            }
        }

        // ===== STATIC NPC =====
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

void drawNPCs(sf::RenderWindow& window, std::string currentMapName, int currentPhase) {
    for (int i = 0; i < npcCount; i++) {

        NPC& npc = allNPCs[i];

        if (npc.currentMap != currentMapName) continue;

        // رسم الـ NPC
        window.draw(npc.sprite);
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
                startDialogue(npc.name, tempLines, lineCount, npc.avatarTexture);

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

bool checkNPCCollision(sf::FloatRect playerBounds, std::string currentMap) {
    for (int i = 0; i < npcCount; i++) {
        NPC& npc = allNPCs[i];

        if (npc.currentMap != currentMap) continue;

        if (playerBounds.intersects(getNPCHitbox(npc))) {
            return true;
        }
    }
    return false;
}

sf::Vector2f getNPCPosition(std::string name) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].name == name) {
            return allNPCs[i].pos;
        }
    }
    return { 0.f, 0.f };
}

void updateNPCPosition(std::string name, sf::Vector2f newPos) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].name == name) {
            allNPCs[i].pos = newPos;
            allNPCs[i].sprite.setPosition(newPos);
            break;
        }
    }
}

// دالة مساعدة عشان نجيب التكستشر بتاع الأفاتار للديالوج
sf::Texture& getNPCAvatar(std::string name) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].name == name) {
            return allNPCs[i].avatarTexture;
        }
    }
    // لو مش لاقي، هيرجع أي حاجة مؤقتاً عشان ميعملش Crash
    return allNPCs[0].avatarTexture;
}


void updateNPCAnimation(std::string name, int direction, float dt) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].name == name) {
            NPC& npc = allNPCs[i];

            // تغيير التكستشر للاتجاه الصح
            npc.sprite.setTexture(npc.walkTextures[direction]);

            // تحديث العداد
            npc.animTimer += dt;
            if (npc.animTimer >= 0.1f) {
                npc.animTimer = 0.f;
                npc.currentFrame = (npc.currentFrame + 1) % 6; // عندك 6 فريمات
            }

            // تحديث المربع اللي بيتقطع من الصورة
            npc.sprite.setTextureRect(sf::IntRect(npc.currentFrame * 68, 0, 68, 68));
            break;
        }
    }
}

// دالة تخلي الـ NPC يقف (Idle)
void setNPCFrame(std::string name, int frame) {
    for (int i = 0; i < npcCount; i++) {
        if (allNPCs[i].name == name) {
            allNPCs[i].sprite.setTextureRect(sf::IntRect(frame * 68, 0, 68, 68));
            break;
        }
    }
}