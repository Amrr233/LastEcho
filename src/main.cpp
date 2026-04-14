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
#include "enemies.h"
#include "DialogueManager.h"

#include <iostream>
#include <cmath>

using namespace sf;
using namespace std;

// =====================
// GLOBALS
// =====================
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
GameMap      myMap;
Game         gameLogic;
inventory    inventorySystem;
AppState     last_state;

bool gameFlags[100] = { false };

// =====================
// MAIN
// =====================
int main() {

    window.create(VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // =====================
    // LOAD MAP
    // =====================
    if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
        return -1;
    }

    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;

    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);

    // =====================
    // INIT STATES
    // =====================
    gState.currentState = STATE_MENU;

    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inventorySystem.invt_init((float)SCREEN_W, (float)SCREEN_H);

    initNPCs();
    initDialogue(); // 🔥 بدل dialogueSystem.init()

    sf::Clock clock;
    audio.playBGM();

    // =====================
    // GAME LOOP
    // =====================
    while (window.isOpen()) {

        gState.deltaTime = clock.restart().asSeconds();
        Event event;

        while (window.pollEvent(event)) {

            if (event.type == Event::Closed)
                window.close();

            // =====================
            // MENU
            // =====================
            if (gState.currentState == STATE_MENU) {
                MenuUpdate(window, gState.currentState);
            }

            // =====================
            // SETTINGS
            // =====================
            else if (gState.currentState == STATE_SETTINGS) {
                SettingsUpdate(window, gState.currentState);
            }

            // =====================
            // GAMEPLAY INPUT
            // =====================
            else if (gState.currentState == STATE_PLAYING) {

                if (event.type == Event::KeyPressed) {

                    if (event.key.code == Keyboard::E) {

                        // 🔥 لو فيه دايلوج → next line
                        if (isOpen) {
                            nextLine();
                        }

                        // 🔥 لو مفيش → interact
                        else {
                            interactWithNPC(player.pos);
                        }
                    }
                }
            }
        }

        // =====================
        // UPDATE
        // =====================
        if (gState.currentState == STATE_PLAYING) {

            gameLogic.update(window, gState.currentState);
            updateDialogue(gState.deltaTime);

            // 🔥 وقف كل حاجة وقت الديالوج
            if (!isOpen) {
                updatePlayer(gState.deltaTime);
                updateEnemies(gState.deltaTime);
                updateNPCs(gState.deltaTime, myMap.mapName, player.pos);
            }
        }

        // =====================
        // DRAW
        // =====================
        window.clear();

        if (gState.currentState == STATE_MENU) {
            MenuDraw(window, gState.currentState);
        }

        else if (gState.currentState == STATE_SETTINGS) {
            settings.draw(window);
        }

        else if (gState.currentState == STATE_PLAYING) {

            // =====================
            // WORLD VIEW
            // =====================
            window.setView(getMapView(myMap));

            drawMap(window, myMap);
            drawNPCs(window, myMap.mapName);
            drawEnemy(window);
            drawPlayer(window);

            // =====================
            // UI VIEW
            // =====================
            window.setView(window.getDefaultView());

            // 🔥 لو دايلوج → اعرضه
            if (isOpen) {
                drawDialogue(window);
            }
            else {
                inventorySystem.invt_draw(window);
            }

            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window);
        }

        window.display();
    }

    return 0;
}