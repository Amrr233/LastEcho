#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "healthbar.h"
#include <iostream>
#include <cmath>

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
    initHealthBar();

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
            window.setView(getMapView(myMap));
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
            // 1. اضبط الكاميرا على مكان اللاعب (العالم المتحرك)
            window.setView(getMapView(myMap));

            // 2. ارسم الحاجات اللي جوه الخريطة (الخريطة + اللاعب)
            drawMap(window, myMap);
            drawPlayer(window);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
                damaging(10);

            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
                healing(50);

            }

            // 3. ارجع للـ View الافتراضي عشان ترسم الـ UI (الواجهة الثابتة)
            window.setView(window.getDefaultView());

            // 4. ارسم الهيلث بار (هينزل فوق الخريطة في مكانه الثابت)
            drawHealthBar(window);
        }

        window.display();
    }

    return 0;
}