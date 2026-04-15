#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "Game.h"
#include "World.h"           // ← ADD THIS (World system)
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

// ==============================
// GLOBALS
// ==============================
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
// GameMap      myMap;        ← REMOVE (old, single map)
World        world;           // ← ADD (new, all maps)
Game         gameLogic;
inventory    inventory;
AppState     last_state;
bool         gameFlags[100] = { false };

// Fade System Variables
float fadeAlpha = 255.0f;
bool isFading = true;
float fadeSpeed = 180.0f;

int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // ════════════════════════════════════════════════════════════════
    // CHANGE 1: Load World (all 16 maps) instead of single map
    // ════════════════════════════════════════════════════════════════
    // OLD:
    // if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
    //     return -1;
    // }

    // NEW:
    if (!worldLoadAllMaps(world)) {
        cout << "CRITICAL ERROR: Failed to load world!" << endl;
        return -1;
    }

    GameMap* currentMap = worldGetCurrentMap(world);
    if (!currentMap) {
        cout << "CRITICAL ERROR: No current map available!" << endl;
        return -1;
    }

    // Spawn coordinates
    float spawnX = 350;
    float spawnY = 900 ;
    // هيعمل سبون قدام السلم
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initweapon(Vector2f(spawnX, spawnY));

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
                    //switching weapons
                    if (event.key.code == sf::Keyboard::F) {
                        weapon.switching(WEAPON_FIST);
                    }
                    if (event.key.code == sf::Keyboard::B) {
                        weapon.switching(WEAPON_BOOK);
                    }
                }
            }
        }

        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {
            // ════════════════════════════════════════════════════════════════
            // CHANGE 2: Get current map each frame
            // ════════════════════════════════════════════════════════════════
            currentMap = worldGetCurrentMap(world);
            if (!currentMap) {
                cout << "ERROR: No current map!" << endl;
                break;
            }

            mainView = updateMapView(mainView, *currentMap, player.pos, gState.deltaTime);
            gameLogic.update(window, gState.currentState);
            inventory.invt_update(window, gState.currentState);

            if (!gameLogic.isPaused) {
                // ════════════════════════════════════════════════════════════════
                // CHANGE 3: Pass world to updatePlayer for collision
                // ════════════════════════════════════════════════════════════════
                updatePlayer(gState.deltaTime, world);  // ← world parameter added

                // NPC update - uses current map name from World
                updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                updateWeapon(gState.deltaTime);

                updateEnemies(gState.deltaTime);

                // ════════════════════════════════════════════════════════════════
                // CHANGE 4: Portal system - use worldSetCurrentMap()
                // ════════════════════════════════════════════════════════════════
                for (auto& p : currentMap->portals) {
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);

                    if (playerBounds.intersects(p.bounds)) {
                        // NEW: Use World system for map switching
                        worldSetCurrentMap(world, p.targetMap);
                        currentMap = worldGetCurrentMap(world);

                        // Teleport player to spawn position
                        player.pos.x = p.spawnPos.x * currentMap->tileSize;
                        player.pos.y = p.spawnPos.y * currentMap->tileSize;
                        player.sprite.setPosition(player.pos);

                        // Fade effect
                        fadeAlpha = 255.0f;
                        isFading = true;

                        std::cout << "[PORTAL] Moved to " << p.targetMap
                                  << " at " << player.pos.x << "," << player.pos.y << std::endl;
                        break;
                    }
                }
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
            // ════════════════════════════════════════════════════════════════
            // CHANGE 5: Draw current map (pointer dereference)
            // ════════════════════════════════════════════════════════════════
            window.setView(mainView);
            drawMap(window, *currentMap);  // ← Dereference pointer

            // Draw NPCs - uses current map name from World
            drawNPCs(window, world.currentMapName);
            drawEnemy(window);
            drawPlayer(window);
            drawWeapons(window);

            // رسم مربعات البوابات للتأكد من مكانها (Debug)
            for (auto& p : currentMap->portals) {
                sf::RectangleShape debugRect(sf::Vector2f(p.bounds.width, p.bounds.height));
                debugRect.setPosition(p.bounds.left, p.bounds.top);
                debugRect.setFillColor(Color(0, 0, 0, 75));
                window.draw(debugRect);
            }

            // عودة للـ View الافتراضية لرسم الـ UI
            window.setView(window.getDefaultView());

            // حوار أم انفنتوري
            if (dialogueSystem.isDialogueActive()) {
                dialogueSystem.draw(window);
            }
            else {
                inventory.invt_draw(window);
            }

            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) healing(10);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) damaging(10);

        if (fadeAlpha > 0) {
            sf::RectangleShape fadeOverlay(sf::Vector2f(SCREEN_W, SCREEN_H));
            fadeOverlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
            window.setView(window.getDefaultView());
            window.draw(fadeOverlay);
        }

        window.display();
    }

    return 0;
}