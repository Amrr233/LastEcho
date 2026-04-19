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
#include "TheLich.h" // ← 1. التأكد من تضمين الهيدر

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
    float spawnY = 900;

    phaseInit(world.phaseSys);
    initPlayer(Vector2f(spawnX, spawnY));

    // ← 2. استدعاء الليتش وتحديده في مكان قريب منك (مثلاً بعيد عنك بـ 200 بكسل)
    initLich(sf::Vector2f(spawnX + 200.f, spawnY - 100.f));

    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initNPCs(world);
    initweapon(Vector2f(spawnX, spawnY));

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
            }
        }

        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {
            Phase& cp = world.phaseSys.allPhases[world.phaseSys.currentPhaseIdx];
            Quest& cq = cp.quests[cp.currentQuestIdx];

            statusTrackerText.setString("Phase: " + cp.phaseTitle + "\nQuest: " + cq.title);

            currentMap = worldGetCurrentMap(world);
            if (!currentMap) break;

            mainView = updateMapView(mainView, *currentMap, player.pos, gState.deltaTime);
            gameLogic.update(window, gState.currentState);
            updateDialogue(gState.deltaTime);
            inv.invt_update(window, gState.currentState, player.pos, gState.deltaTime);

            if (!gameLogic.isPaused && !isDialogueActive()) {
                updatePlayer(gState.deltaTime, world);
                checkDialogueReward(world.phaseSys);

                // ← 3. تحديث الليتش (كود الملاحقة والضرب)
                sf::FloatRect playerHitbox = player.sprite.getGlobalBounds();
                updateLich(gState.deltaTime, player.pos, playerHitbox);

                updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                updateWeapon(gState.deltaTime);
                updateEnemies(gState.deltaTime);

                for (auto& p : currentMap->portals) {
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);
                    if (playerBounds.intersects(p.bounds)) {
                        if (p.targetMap == "lobby" && world.phaseSys.currentPhaseIdx == 0) {
                            warningMessage.setString("The gate is locked. Talk to the security guard!");
                            warningTimer = 1.5f;
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
            if (fadeAlpha <= 0) { fadeAlpha = 0; isFading = false; }
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

            // ← 4. رسم الليتش (بعد الماب وقبل اللاعب والـ UI)
            drawLich(window);

            drawNPCs(window, world.currentMapName, world.phaseSys.currentPhaseIdx);
            drawEnemy(window);
            drawPlayer(window);

            if (inv.feedbackTimer > 0) {
                window.draw(inv.feedbackSprite);
                for (int i = 0; i < 5; i++) window.draw(inv.sparkles[i]);
            }
            drawWeapons(window);

            window.setView(window.getDefaultView());
            if (isDialogueActive()) drawDialogue(window);
            else inv.invt_draw(window);

            drawHealthBar(window);
            if (warningTimer > 0) {
                sf::Text popUp;
                popUp.setFont(font);
                popUp.setString("The gate is locked. Talk to the security guard!");
                popUp.setCharacterSize(24);
                popUp.setFillColor(sf::Color::Red);
                popUp.setPosition(SCREEN_W/2.0f - popUp.getGlobalBounds().width/2.0f, 200.f);
                window.draw(popUp);
                warningTimer -= gState.deltaTime;
            }
            drawXPBar(window);
            gameLogic.draw(window);
        }

        window.setView(window.getDefaultView());
        statusTrackerText.setFont(font);
        statusTrackerText.setCharacterSize(20);
        statusTrackerText.setFillColor(sf::Color::White);
        statusTrackerText.setOutlineColor(sf::Color::Black);
        statusTrackerText.setOutlineThickness(2);
        statusTrackerText.setPosition(20.f, 140.f);
        window.draw(statusTrackerText);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) healing(10);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) damaging(10);

        if (fadeAlpha > 0) {
            sf::RectangleShape fadeOverlay(sf::Vector2f(SCREEN_W, SCREEN_H));
            fadeOverlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
            window.draw(fadeOverlay);
        }

        window.display();
    }
    return 0;
}