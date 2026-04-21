#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "Game.h"
#include "World.h"
#include <iostream>
#include "healthbar.h"
#include "inventory.h"
#include "XPBar.h"
#include "NPC.h"
#include "enemies.h"
#include "DialogueManager.h"
#include "phase.h"
#include "Cutscene.h"
#include "GuitarMiniGame.h"

using namespace sf;
using namespace std;

// ==============================
// GLOBALS
// ==============================
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
World        world;
Game         gameLogic;
inventory    inv;
AppState     last_state;
bool         gameFlags[100] = { false };
Text warningMessage;
float warningTimer = 0.0f;
float fadeAlpha = 255.0f;
bool isFading = true;
float fadeSpeed = 180.0f;
Font font;
Text statusTrackerText;

int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        std::cout << "ERROR: Font not found!" << std::endl;
    }

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

    // Initializations
    initCutsceneSystem();
    phaseInit(world.phaseSys);
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initNPCs(world);
    initweapon(Vector2f(spawnX, spawnY));
    initGuitar();  // 🔥 Initialize guitar

    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inv.invt_init((float)SCREEN_W, (float)SCREEN_H);
    initDialogue();

    Clock clock;
    View mainView;
    mainView.setSize(SCREEN_W, SCREEN_H);

    while (window.isOpen()) {
        // std::cout << "X: " << player.pos.x << " Y: " << player.pos.y << std::endl;
        gState.deltaTime = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            // ════════════════════════════════════════════════════════════════
            // GUITAR MOUSE CLICK HANDLING
            // ════════════════════════════════════════════════════════════════
            if (event.type == Event::MouseButtonPressed) {
                if (isGuitarOpen()) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(
                        sf::Mouse::getPosition(window)
                    );
                    handleGuitarClick(mousePos);
                }
            }

            // ════════════════════════════════════════════════════════════════
            // KEY PRESS HANDLING
            // ════════════════════════════════════════════════════════════════
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Tab) {
                    if (!isGuitarOpen()) {
                        openGuitarFreePlay();
                        centerGuitarUI(window); // بنسنتر أول ما نفتح
                    } else closeGuitar();
                }

                if (isGuitarOpen() && event.key.code == sf::Keyboard::Q) {
                    if (g_guitar.mode == GUITAR_FREE) {
                        // باتيرن تجريبي: وتر 0 فريت 1، وتر 0 فريت 2، وتر 1 فريت 3
                        GuitarNote pattern[12] = {{0, 7}, {1, 4}, {1, 4} , {0, 7}, {0, 7},{1, 4},{1, 4},{0, 7},{0, 7},{1,4},{1,5},{1,4} };
                        openGuitarQuest(pattern, 12, 60.0f); // 60 ثانية
                    } else {
                        openGuitarFreePlay();
                    }
                }
            }

            if (event.type == sf::Event::Closed)
                window.close();

            if (gState.currentState == STATE_MENU) {
                MenuUpdate(window, gState.currentState);
            }
            else if (gState.currentState == STATE_SETTINGS) {
                SettingsUpdate(window, gState.currentState);
            }
            else if (gState.currentState == STATE_PLAYING) {
                // منع التفاعل اليدوي لو فيه كت سين شغالة
                if (!isCutsceneActive() && !isGuitarOpen()) {
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                        if (isDialogueActive()) {
                            nextLine();
                        }
                        else {
                            string npcName = getNearbyNPCName(player.pos, world.currentMapName);
                            if (npcName != "") {
                                updatePhaseLogic(world.phaseSys, npcName);
                            }
                        }
                    }
                } else {
                    // لو فيه كت سين أو جيتار وفيها حوار، الزرار E يقلب الديالوج بس
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                        if (isDialogueActive()) nextLine();
                    }
                }
            }
        }

        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {
            // ════════════════════════════════════════════════════════════════
            // UPDATE GUITAR (EVERY FRAME)
            // ════════════════════════════════════════════════════════════════
            if (isGuitarOpen()) {
                updateGuitar(gState.deltaTime);
            }

            updateCutscene(gState.deltaTime);
            Phase& cp = world.phaseSys.allPhases[world.phaseSys.currentPhaseIdx];
            Quest& cq = cp.quests[cp.currentQuestIdx];
            statusTrackerText.setString("Phase: " + cp.phaseTitle + "\n" + "Quest: " + cq.title);

            currentMap = worldGetCurrentMap(world);
            if (!currentMap) break;

            mainView = updateMapView(mainView, *currentMap, player.pos, gState.deltaTime);

            gameLogic.update(window, gState.currentState);
            updateDialogue(gState.deltaTime);
            inv.invt_update(window, gState.currentState, player.pos, gState.deltaTime);

            // بنوقف حركة اللاعب والوحوش لو فيه كت سين أو جيتار
            if (!gameLogic.isPaused && !isDialogueActive() && !isCutsceneActive() && !isGuitarOpen()) {
                updatePlayer(gState.deltaTime, world);
                checkDialogueReward(world.phaseSys);
                updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                updateWeapon(gState.deltaTime);
                updateEnemies(gState.deltaTime);

                for (auto& p : currentMap->portals) {
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);
                    if (playerBounds.intersects(p.bounds)) {
                        if (p.targetMap == "lobby" && world.phaseSys.currentPhaseIdx == 0 && world.phaseSys.allPhases[0].currentQuestIdx < 2) {
                            warningMessage.setString("The gate is locked. Talk to the security guard!") ;
                            warningTimer = 1.0f;
                            break;
                        }
                        worldSetCurrentMap(world, p.targetMap);
                        currentMap = worldGetCurrentMap(world);
                        player.pos.x = p.spawnPos.x * currentMap->tileSize;
                        player.pos.y = p.spawnPos.y * currentMap->tileSize;
                        player.sprite.setPosition(player.pos);
                        fadeAlpha = 255.0f;
                        isFading = true;
                        break;
                    }
                }
            }
        }

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
            window.setView(mainView);
            drawMap(window, *currentMap);
            drawNPCs(window, world.currentMapName, world.phaseSys.currentPhaseIdx);
            drawEnemy(window);
            drawPlayer(window);

            drawCutsceneOverlay(window, font);

            if (inv.feedbackTimer > 0) {
                window.draw(inv.feedbackSprite);
                for (int i = 0; i < 5; i++) window.draw(inv.sparkles[i]);
            }
            drawWeapons(window);

            window.setView(window.getDefaultView());

            if (isDialogueActive()) {
                drawDialogue(window);
            }
            else {
                inv.invt_draw(window);
            }

            drawHealthBar(window);

            if (warningTimer > 0) {
                sf::Text popUp;
                popUp.setFont(font);
                popUp.setString(warningMessage.getString());
                popUp.setCharacterSize(24);
                popUp.setFillColor(sf::Color::Red);
                popUp.setOutlineColor(sf::Color::Black);
                popUp.setOutlineThickness(2);
                popUp.setPosition(SCREEN_W/2.0f - popUp.getGlobalBounds().width/2.0f, 200.f);
                window.draw(popUp);
                warningTimer -= gState.deltaTime;
            }

            drawXPBar(window);
            gameLogic.draw(window);

            // ════════════════════════════════════════════════════════════════
            // DRAW GUITAR (ON TOP OF EVERYTHING)
            // ════════════════════════════════════════════════════════════════
            if (isGuitarOpen()) {
                drawGuitar(window);
            }
        }

        window.setView(window.getDefaultView());
        statusTrackerText.setFont(font);
        statusTrackerText.setCharacterSize(20);
        statusTrackerText.setFillColor(sf::Color::White);
        statusTrackerText.setOutlineColor(sf::Color::Black);
        statusTrackerText.setOutlineThickness(2);
        statusTrackerText.setPosition(20.f, 140.f);
        window.draw(statusTrackerText);

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