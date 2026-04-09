#include "NPC.h"      // تعريف كلاس الـ NPC
#include "Data.h"     // بيانات اللعبة
#include <cmath>      // دوال رياضية (sqrt)
#include <iostream>   // للطباعة

std::vector<NPC> allNPCs; // فيكتور بيخزن كل الـ NPCs

enum Direction { SOUTH = 0, NORTH = 1, WEST = 2, EAST = 3 }; // الاتجاهات (للأنيميشن)


// =====================================
// INIT (تهيئة الـ NPCs)
// =====================================
void initNPCs() {
    allNPCs.clear(); // مسح أي NPC قديم

    // =========================
    // NPC متحرك
    // =========================
    NPC student;
    student.name = "Student"; // اسم الشخصية

    // تحميل صور الحركة لكل اتجاه
    student.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking.south.png");
    student.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking.north.png");
    student.walkTextures[WEST].loadFromFile("assets/sprites/player/walking.west.png");
    student.walkTextures[EAST].loadFromFile("assets/sprites/player/walking.east.png");

    student.isStatic = false; // متحرك
    student.speed = 90.f;     // السرعة

    // مسار الحركة (نقاط)
    student.waypoints = {
        {300, 800},
        {600, 800},
        {600, 900},
        {300, 900},
    };

    student.pos = student.waypoints[0]; // يبدأ من أول نقطة
    student.currentMap = "outside";     // يظهر في الماب ده
    student.dialogues.push_back({"Hey! I'm touring the campus.", -1});

    allNPCs.push_back(student); // إضافة للقائمة


    // =========================
    // NPC ثابت
    // =========================
    NPC hadry_static;
    hadry_static.name = "hadry";

    // تحميل كل الاتجاهات عشان يقدر يلف وشه ليك لما تقرب منه
    hadry_static.walkTextures[SOUTH].loadFromFile("assets/sprites/player/walking.south.png");
    hadry_static.walkTextures[NORTH].loadFromFile("assets/sprites/player/walking.north.png");
    hadry_static.walkTextures[WEST].loadFromFile("assets/sprites/player/walking.west.png");
    hadry_static.walkTextures[EAST].loadFromFile("assets/sprites/player/walking.east.png");

    hadry_static.isStatic = true;     // ثابت
    hadry_static.pos = {800, 800};    // مكانه
    hadry_static.currentMap = "outside";
    hadry_static.dialogues.push_back({"Welcome to the Faculty.", -1});

    allNPCs.push_back(hadry_static);


    // =========================
    // إعدادات عامة لكل NPC
    // =========================
    for (auto& npc : allNPCs) {

        npc.sprite.setTexture(npc.walkTextures[SOUTH]); // أول صورة

        npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68)); // أول frame

        npc.sprite.setOrigin(34.f, 34.f); // مركز السبرايت

        npc.sprite.setScale(1.8f, 1.8f); // تكبير

        npc.sprite.setPosition(npc.pos); // تحديد المكان

        npc.currentFrame = 0; // بداية الأنيميشن
        npc.animTimer = 0.f;
    }
}


// =====================================
// UPDATE (الحركة + collision)
// =====================================
void updateNPCs(float deltaTime, std::string currentMapName, sf::Vector2f playerPos) {

    for (auto& npc : allNPCs) {

        if (npc.currentMap != currentMapName) continue; // لو مش في نفس الماب

        if (!npc.isStatic && !npc.waypoints.empty()) { // لو متحرك

            sf::Vector2f target = npc.waypoints[npc.currentWaypoint]; // الهدف الحالي

            sf::Vector2f moveVec = target - npc.pos; // اتجاه الحركة

            float distance = std::sqrt(moveVec.x * moveVec.x + moveVec.y * moveVec.y); // المسافة

            if (distance > 2.0f) { // لسه ماوصلش

                sf::Vector2f dir = moveVec / distance; // اتجاه normalized

                // نحسب المكان الجديد قبل ما نتحرك
                sf::Vector2f nextPos = npc.pos + dir * npc.speed * deltaTime;

                // ===============================
                // COLLISION مع اللاعب
                // ===============================

                sf::FloatRect npcBounds = npc.sprite.getGlobalBounds(); // بوكس الـ NPC

                // 🔥 تعديل الحدود: قللنا العرض (20) وزودنا الطول (48)
                sf::FloatRect playerBounds(
                    playerPos.x - 10, // نص العرض (20/2)
                    playerPos.y - 24, // نص الطول (48/2)
                    20,               // العرض الكلي
                    48                // الطول الكلي
                );

                // نحرك بوكس الـ NPC للمكان الجديد
                npcBounds.left = nextPos.x - npcBounds.width / 2;
                npcBounds.top  = nextPos.y - npcBounds.height / 2;

                // لو حصل تصادم
                if (npcBounds.intersects(playerBounds)) {

                    // وقف الأنيميشن
                    npc.animTimer = 0.f;
                    npc.currentFrame = 0;

                    // 🔥 الجديد: خليه يبص للاعب لما يخبط فيه ويقف
                    sf::Vector2f diff = playerPos - npc.pos;
                    if (std::abs(diff.x) > std::abs(diff.y)) {
                        npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                    } else {
                        npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                    }

                    // يخليه واقف
                    npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
                }
                else {
                    // مفيش تصادم → يتحرك
                    npc.pos = nextPos;

                    // ===============================
                    // تحديد الاتجاه
                    // ===============================
                    if (std::abs(dir.x) > std::abs(dir.y)) {
                        npc.sprite.setTexture(
                            dir.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]
                        );
                    } else {
                        npc.sprite.setTexture(
                            dir.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]
                        );
                    }

                    // ===============================
                    // الأنيميشن
                    // ===============================
                    npc.animTimer += deltaTime;

                    if (npc.animTimer >= 0.1f) {
                        npc.animTimer = 0.f;
                        npc.currentFrame = (npc.currentFrame + 1) % 6;
                    }

                    npc.sprite.setTextureRect(
                        sf::IntRect(npc.currentFrame * 68, 0, 68, 68)
                    );
                }

            } else {
                // وصل للنقطة → يروح للي بعدها
                npc.currentWaypoint = (npc.currentWaypoint + 1) % npc.waypoints.size();
            }
        } else if (npc.isStatic) {
            // 🔥 الجديد: لو الـ NPC ثابت واللاعب قريب منه، يلف وشه يتابعه
            float distToPlayer = std::sqrt(std::pow(npc.pos.x - playerPos.x, 2) + std::pow(npc.pos.y - playerPos.y, 2));

            if (distToPlayer < 120.0f) {
                sf::Vector2f diff = playerPos - npc.pos;
                if (std::abs(diff.x) > std::abs(diff.y)) {
                    npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
                } else {
                    npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
                }
            } else {
                // 🔥 الجديد: لما تبعد عنه، يرجع يبص في مكانه (SOUTH)
                npc.sprite.setTexture(npc.walkTextures[SOUTH]);
            }

            // دايماً فريم الوقوف للـ Static
            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));
        }

        npc.sprite.setPosition(npc.pos); // تحديث المكان النهائي
    }
}


// =====================================
// DRAW (الرسم)
// =====================================
void drawNPCs(sf::RenderWindow& window, std::string currentMapName) {
    for (const auto& npc : allNPCs) {

        if (npc.currentMap == currentMapName) { // نفس الماب
            window.draw(npc.sprite); // ارسمه
        }
    }
}


// =====================================
// INTERACTION (التفاعل)
// =====================================
void interactWithNPC(sf::Vector2f playerPos) {

    for (auto& npc : allNPCs) {

        // حساب المسافة بين اللاعب و NPC
        float dist = std::sqrt(
            std::pow(npc.pos.x - playerPos.x, 2) +
            std::pow(npc.pos.y - playerPos.y, 2)
        );

        if (dist < 75.0f) { // لو قريب

            // 🔥 الجديد: خليه يبص للاعب فوراً وقت الكلام
            sf::Vector2f diff = playerPos - npc.pos;
            if (std::abs(diff.x) > std::abs(diff.y)) {
                npc.sprite.setTexture(diff.x > 0 ? npc.walkTextures[EAST] : npc.walkTextures[WEST]);
            } else {
                npc.sprite.setTexture(diff.y > 0 ? npc.walkTextures[SOUTH] : npc.walkTextures[NORTH]);
            }
            npc.sprite.setTextureRect(sf::IntRect(0, 0, 68, 68));

            // طباعة الحوار
            std::cout << npc.name << ": "
                      << npc.dialogues[0].text << std::endl;

            return; // يقف عند أول واحد
        }
    }
}