#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "Game.h" // ضفنا الهيدر بتاع البوز
#include <iostream>
#include <cmath>

#include "healthbar.h"
#include "inventory.h"
#include "XPBar.h"

using namespace sf;
using namespace std;

// ==============================
// GLOBALS
// ==============================
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
GameMap      myMap; // دلوقتى عبارة عن struct عادي جداً
Game         gameLogic;  // تعريف كائن اللعبة للتحكم في البوز
inventory    inventory;
int main() {
    // 1. إنشاء النافذة
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // 2. تحميل الماب (باستخدام الدالة المستقلة)
    if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
        cout << "CRITICAL ERROR: Failed to load map file!" << endl;
        return -1;
    }

    // 3. حساب أبعاد الماب لوضع اللاعب في المنتصف
    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;
    initPlayer(Vector2f(spawnX, spawnY));

    // إعدادات المنيو والصوت والبوز
    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H); // تحميل ملفات البوز

    sf::Clock clock;
    audio.playBGM();

    // ==============================
    // MAIN GAME LOOP
    // ==============================
    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // التعامل مع الـ Events في المنيو والإعدادات
            if (gState.currentState == STATE_MENU) {
                MenuUpdate(window, gState.currentState);
            }
            else if (gState.currentState == STATE_SETTINGS) {
                SettingsUpdate(window, gState.currentState);
            }
        }

        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {
            gameLogic.update(window, gState.currentState);

            if (!gameLogic.isPaused) { // لو مش عامل بوز، كمل تحديث اللاعب
                updatePlayer(gState.deltaTime);
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
            // أ. ضبط الكاميرا (الـ View) بناءً على الماب الحالية
            window.setView(getMapView(myMap));
            // ب. رسم الماب واللاعب
            drawMap(window, myMap);
            drawPlayer(window);

            // ج. رسم الـ UI (لازم نرجع للـ Default View عشان الحاجات دي تفضل ثابتة في الشاشة)
            window.setView(window.getDefaultView());
            // 3. نادي رسم الانفنتوري
            inventory.invt_draw(window);
            in.draw(window);
            drawHealthBar(window);
            drawXPBar(window);

            // د. رسم زرار ولوحة البوز (تحت الهيلث والاكسبي عشان تظهر فوقهم)
            gameLogic.draw(window);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            healing(10);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            damaging(10);
        }
        window.display();
    }

    return 0;
}