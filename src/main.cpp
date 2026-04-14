#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "Game.h"
#include <iostream>
#include "healthbar.h"
#include "inventory.h"
#include "XPBar.h"
#include "NPC.h"
#include <iostream>
#include <cmath>
#include "enemies.h"
#include "DialogueManager.h"

using namespace sf;
using namespace std;

RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
GameMap      myMap;
Game         gameLogic;
inventory    inventory;
AppState     last_state;
bool         gameFlags[100] = { false };
// Fade System Variables
float fadeAlpha = 255.0f; // بنبدأ بـ 255 عشان أول ماب تفتح بسواد بيختفي
bool isFading = true;    // هل إحنا في حالة التلاشي دلوقتي؟
float fadeSpeed = 180.0f; // سرعة السواد (ممكن تزودها أو تقللها)

int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // 2. تحميل الماب الافتراضية (البداية)
    if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
        return -1;
    }

    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);

    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inventory.invt_init((float)SCREEN_W, (float)SCREEN_H);
    initNPCs();
    dialogueSystem.init();

    Clock clock;
    audio.playBGM();

    // تعريف الفيو
    View mainView;
    mainView.setSize(SCREEN_W, SCREEN_H);

    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (gState.currentState == STATE_MENU) {
                MenuUpdate(window, gState.currentState);
            }
            else if (gState.currentState == STATE_SETTINGS) {
                SettingsUpdate(window, gState.currentState);
            }
            else if (gState.currentState == STATE_PLAYING) {
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::E) {
                        if (dialogueSystem.isDialogueActive()) {
                            dialogueSystem.nextLine();
                        } else {
                            interactWithNPC(player.pos);
                        }
                    }
                }
            }
        }
        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {
            mainView = updateMapView(mainView, myMap, player.pos, gState.deltaTime);
            gameLogic.update(window, gState.currentState);
            // tool bar
            inventory.invt_update(window, gState.currentState);

            if (!gameLogic.isPaused) {
                updatePlayer(gState.deltaTime);
                updateNPCs(gState.deltaTime, myMap.mapName, player.pos);
                updateEnemies(gState.deltaTime);
                // ========================================================
                // سيستم الانتقال بين المابس وحركات وبركات
                for (auto& p : myMap.portals) {
                    // هنعمل مربع يدوي للاعب بناءً على مكانه الحالي ومقاسه (48x48)
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);

                    // بنتشيك هل مربع اللاعب بيخبط في مربع البوابة
                    if (playerBounds.intersects(p.bounds)) {
                        // 1. خزن الإحداثيات والمسار في متغيرات مؤقتة الأول
                        string nextMap = p.targetMap;
                        float nextX = p.spawnPos.x;
                        float nextY = p.spawnPos.y;

                        string fullPath = "assets/maps/" + nextMap + "/" + nextMap + ".json";

                        // 2. حمل الماب الجديدة
                        if (loadMapFromJSON(myMap, fullPath)) {
                            // 3. استخدم المتغيرات المؤقتة اللي خزناها من الـ JSON بتاع الـ outside
                            player.pos.x = nextX * myMap.tileSize;
                            player.pos.y = nextY * myMap.tileSize;
                            player.sprite.setPosition(player.pos);
                            fadeAlpha = 255.0f; // رجع السواد كامل
                            isFading = true;    // فعل الـ Fade عشان يبدأ يفتح ببطء

                            std::cout << "[Fixed]: Moved to " << nextMap << " at " << player.pos.x << "," << player.pos.y << std::endl;
                            break;
                        }
                    }
                }
                // ========================================================
            }
        }
        // تحديث قيمة الـ Fade
        if (isFading) {
            fadeAlpha -= fadeSpeed * gState.deltaTime;
            if (fadeAlpha <= 0) {
                fadeAlpha = 0;
                isFading = false;
            }
        }

        // --- DRAW LOGIC ---
        window.clear();

        if (gState.currentState == STATE_MENU) {
            MenuDraw(window, gState.currentState);
        }
        else if (gState.currentState == STATE_SETTINGS) {
            settings.draw(window);
        }
        else if (gState.currentState == STATE_PLAYING) {
            // تظبيط الكاميرا على الماب الجديدة
            window.setView(mainView);
            drawMap(window, myMap);
            drawNPCs(window, myMap.mapName);
            drawEnemy(window);
            drawPlayer(window);
            // رسم مربعات البوابات للتأكد من مكانها (Debug)
            for (auto& p : myMap.portals) {
                sf::RectangleShape debugRect(sf::Vector2f(p.bounds.width, p.bounds.height));
                debugRect.setPosition(p.bounds.left, p.bounds.top);
                debugRect.setFillColor(sf::Color(255, 0, 0, 100));
                window.draw(debugRect);
            }

            // 2. رسم الواجهة (الكاميرا الثابتة)
            window.setView(window.getDefaultView());

            // 🔥 هنا اللعبة: لو فيه حوار شغال، اظهر الحوار واخفي الانفنتوري
            if (dialogueSystem.isDialogueActive()) {
                dialogueSystem.draw(window);
            }
            else {
                // لو مفيش حوار، اظهر الانفنتوري عادي
                inventory.invt_draw(window);
            }

            // باقي الـ UI (الهيلث والـ XP يفضلوا ظاهرين أو اخفيهم بنفس الطريقة لو تحب)
            // العودة للـ View الافتراضية لرسم الـ UI فوق الماب
            inventory.invt_draw(window);
            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) healing(10);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) damaging(10);
        if (fadeAlpha > 0) {
            sf::RectangleShape fadeOverlay(sf::Vector2f(SCREEN_W, SCREEN_H));
            fadeOverlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));

            // تأكد إننا بنرسم على الـ Default View عشان يغطي الشاشة الثابتة مش الكاميرا المتحركة
            window.setView(window.getDefaultView());
            window.draw(fadeOverlay);
        }
        window.display();
    }
    return 0;
}