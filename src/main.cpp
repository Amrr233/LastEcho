#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include <iostream>
#include <cmath>

#include "healthbar.h"
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
    // بما إننا شغالين Struct، بنوصل للبيانات مباشرة
    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;
    initPlayer(Vector2f(spawnX, spawnY));

    // إعدادات المنيو والصوت
    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);

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
            updatePlayer(gState.deltaTime);
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
            // بننادي الدالة ونبعت لها الـ struct
            window.setView(getMapView(myMap));
            // ب. رسم الماب (الـ 3 ليرات)
            drawMap(window, myMap);
            // ج. رسم اللاعب
            drawPlayer(window);
            drawHealthBar(window);
            drawXPBar(window);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            healing(10);
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            damaging(10);
        }
        window.display();
    }
//xxxx
    return 0;
}