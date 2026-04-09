#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "Game.h"
#include "healthbar.h"
#include "inventory.h"
#include "XPBar.h"
#include "NPC.h"
#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

// ==============================
// GLOBALS (Definitions)
// ==============================
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
GameMap      myMap;
Game         gameLogic;
inventory    inventory;
bool         gameFlags[100] = { false }; // حجز مكان المصفوفة

int main() {
    // 1. إنشاء النافذة
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // 2. تحميل الماب واللاعب
    if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
        cout << "CRITICAL ERROR: Failed to load map file!" << endl;
        return -1;
    }

    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;
    initPlayer(Vector2f(spawnX, spawnY));

    // 3. إعداد الأنظمة
    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inventory.invt_init((float)SCREEN_W, (float)SCREEN_H);
    initNPCs(); // تجهيز الـ NPCs

    sf::Clock clock;
    audio.playBGM();

    // ==============================
    // MAIN GAME LOOP (واحدة بس!)
    // ==============================
    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // إيفنت التفاعل مع الـ NPC
            if (gState.currentState == STATE_PLAYING && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::E) {
                    interactWithNPC(player.pos);
                }
            }

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
            inventory.invt_update(window, gState.currentState);

            if (!gameLogic.isPaused) {
                updatePlayer(gState.deltaTime);
                updateNPCs(gState.deltaTime, myMap.mapName, player.pos); // تحديث حركة الـ NPCs
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
            // 1. رسم العالم (View الكاميرا)
            window.setView(getMapView(myMap));
            drawMap(window, myMap);
            drawNPCs(window, myMap.mapName); // رسم الـ NPCs
            drawPlayer(window);

            // 2. رسم الـ UI (View الشاشة الثابت)
            window.setView(window.getDefaultView());
            inventory.invt_draw(window);
            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window); // لوحة البوز
        }

        window.display();
    }

    return 0;
}