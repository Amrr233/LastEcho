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
#include "enemies.h"
#include <iostream>
#include <cmath>

#include "healthbar.h"
#include "inventory.h"
#include "XPBar.h"
#include <enemies.h>

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
bool         gameFlags[100] = { false };

int main() {
    // 1. Create Window
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // 2. تحميل الماب واللاعب
    // 2. Load Map
    if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
        cout << "CRITICAL ERROR: Failed to load map file!" << endl;
        return -1;
    }

    // 3. Setup Spawn and Entities
    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;

    initPlayer(Vector2f(spawnX, spawnY));

    // مكان العدو قريب منك عشان تشوفه
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);

    // 3. إعداد الأنظمة
    initEnemy(0, sf::Vector2f(500.f, 400.f), BASIC_ENEMY);

    // 4. Initialize Systems
    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inventory.invt_init((float)SCREEN_W, (float)SCREEN_H);
    initNPCs();

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

            // تفاعل NPC
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
                updateNPCs(gState.deltaTime, myMap.mapName, player.pos);
                updateEnemies(gState.deltaTime); // تحديث الأعداء عشان يتحركوا
                updateEnemies(gState.deltaTime);
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
            // رسم العالم
            window.setView(getMapView(myMap));
            drawMap(window, myMap);
            drawNPCs(window, myMap.mapName);
            drawEnemy(window);
            drawPlayer(window);

            // C. Debug Hitbox (المصلح)
            if (player.currentState == ATTACKING && player.currentFrame == 3) {
                sf::FloatRect rect = attackHitBox();
                sf::RectangleShape debugBox(sf::Vector2f(rect.width, rect.height));
                debugBox.setPosition(rect.left, rect.top);
                // التصحيح لـ setFillColor بدل setFillOptions
            // A. Set Camera View
            window.setView(getMapView(myMap));

            // B. Draw World Objects
            drawMap(window, myMap);

            // DRAW ENEMIES (Added fix to ensure they show up)
            drawEnemy(window);

            drawPlayer(window);

            // C. Debug Hitbox
            if (player.currentState == ATTACKING) {
                sf::FloatRect rect = attackHitBox();
                sf::RectangleShape debugBox(sf::Vector2f(rect.width, rect.height));
                debugBox.setPosition(rect.left, rect.top);
                debugBox.setFillColor(sf::Color(255, 0, 0, 120));
                window.draw(debugBox);
            }

            // رسم الواجهة
            // D. Draw UI (Switch back to Default View)
            window.setView(window.getDefaultView());
            inventory.invt_draw(window);
            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window);
        }

        // Temporary testing inputs
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