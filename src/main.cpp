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

int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

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

    sf::Clock clock;
    audio.playBGM();

    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();
        sf::Event event;
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

        if (gState.currentState == STATE_PLAYING) {
            gameLogic.update(window, gState.currentState);
            inventory.invt_update(window, gState.currentState);

            if (!gameLogic.isPaused) {
                updatePlayer(gState.deltaTime);
                updateNPCs(gState.deltaTime, myMap.mapName, player.pos);
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
            // 1. رسم العالم (الكاميرا المتحركة)
            window.setView(getMapView(myMap));
            drawMap(window, myMap);
            drawNPCs(window, myMap.mapName);
            drawEnemy(window);
            drawPlayer(window);

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
            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window);
        }

        window.display();
    }
    return 0;
}