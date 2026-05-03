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
#include "chest.h"
#include "Cutscene.h"
#include "GuitarMiniGame.h"
#include "boss.h"
#include "HiddenWordsminigame.h"
#include "HintSystem.h"

using namespace sf;
using namespace std;

// --- Global Variables ---
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audioManger;
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
sf::Texture interactBoxTex;
sf::Sprite  interactBoxSprite;
sf::Text    interactPrompt;
extern GuitarGame g_guitar;
float spawnX = 350;
float spawnY = 900;

int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        cout << "ERROR: Font not found!" << endl;
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

    interactBoxTex.loadFromFile("assets/sprites/items/Text_Box.png");
    interactBoxSprite.setTexture(interactBoxTex);
    interactBoxSprite.setScale(0.35f, 0.35f);

    float boxW = interactBoxSprite.getGlobalBounds().width;
    float boxH = interactBoxSprite.getGlobalBounds().height;
    interactBoxSprite.setPosition(SCREEN_W - boxW - 170.f, SCREEN_H - boxH - 20.f);

    interactPrompt.setFont(font);
    interactPrompt.setString("Press E to interact");
    interactPrompt.setCharacterSize(16);
    interactPrompt.setFillColor(sf::Color(60, 30, 10));
    interactPrompt.setOutlineColor(sf::Color::Black);
    interactPrompt.setOutlineThickness(1);

    float spawnX = 350;
    float spawnY = 900;

    // --- Systems Initialization ---
    initCutsceneSystem();
    phaseInit(world.phaseSys);
    initHintSystem(world.hintSys);
    //*******************************************************************
    //world.hintSys.hintsUnlocked = 1;
    //world.hintSys.hasNewHint = true;
    //-------------------------------------------------------------------
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initBoss();
    // To test right now — remove this line later when cutscene triggers it
    // Init section — replace spawnBoss with:
    startRound(1);
    initNPCs(world);
    initChest(sf::Vector2f(100.f, 150.f), "sclab");
    initweapon(Vector2f(spawnX, spawnY));
    initGuitar();
    initDialogue();

    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inv.invt_init((float)SCREEN_W, (float)SCREEN_H);

    // --- Hidden Words Minigame Setup ---
    MovieReview myReview;
    initReviewGame(myReview);
    sf::Texture terminalTex;
    terminalTex.loadFromFile("Assets/gameplay/terminalasset.png");
    sf::Sprite terminalSprite(terminalTex);
    terminalSprite.setScale(0.7f, 0.7f);
    terminalSprite.setOrigin(terminalSprite.getLocalBounds().width / 2.f, terminalSprite.getLocalBounds().height / 2.f);
    terminalSprite.setPosition(SCREEN_W / 2.f, SCREEN_H / 2.f);
    sf::Font terminalFont;
    terminalFont.loadFromFile("Assets/fonts/pixelsix00.ttf");
    bool isMinigameActive = false;

    Clock clock;
    View mainView;
    mainView.setSize(SCREEN_W, SCREEN_H);

    while (window.isOpen()) {
        cout << "x  : " << player.pos.x << "  y  : " << player.pos.y << endl;
        gState.deltaTime = clock.restart().asSeconds();
        Event event;

        // ════════════════════════════════════════════════════════════════
        // 1. EVENT LOOP
        // ════════════════════════════════════════════════════════════════
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // --- A. Minigame Priority ---
            if (isMinigameActive) {
                updateReviewInput(event, myReview);
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) isMinigameActive = false;
                if (myReview.isCleared && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) isMinigameActive = false;
                continue;
            }
            else if (gState.currentState == STATE_SETTINGS) {
                SettingsUpdate(window, gState.currentState);
            }
            else if (gState.currentState == STATE_PLAYING) {
                if (!isCutsceneActive() && !isGuitarOpen()) {
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                        if (isDialogueActive()) {
                            nextLine();
                        }
                        else if (tryOpenChest(player.pos, world.currentMapName)) {
                            // chest opened
                        }
                        else if (canPickupString(world.phaseSys, player.pos, world.currentMapName, world.hintSys)) {
                            pickupString(world.phaseSys, player.pos, world.currentMapName, world.hintSys);
                        }
                        else {
                            string npcName = getNearbyNPCName(player.pos, world.currentMapName);
                            if (npcName != "") {
                                updatePhaseLogic(world.phaseSys, npcName, world.hintSys);
                            }
                        }
                    }
                    if (event.key.code == sf::Keyboard::F) {
                        weapon.switching(WEAPON_FIST);
                    }
                    if (event.key.code == sf::Keyboard::B) {
                        weapon.switching(WEAPON_BOOK);
                    }
                }
                // Other Keys (B, N)
                if (event.type == Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::B) weapon.switching(WEAPON_BOOK);
                    if (event.key.code == sf::Keyboard::N) {
                        inv.triggerPickupEffect("assets/items/note.png");
                        inv.addItem("note", "assets/items/note.png");
                    }
                }
            }

        // ── UPDATE ───────────────────────────────────────────────────────
        if (gState.currentState == STATE_PLAYING) {
            if (isGuitarOpen()) {
                if (event.type == Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                    handleGuitarClick(window, sf::Mouse::getPosition(window));

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                    if (g_guitar.mode == GUITAR_FREE) {
                        GuitarNote pattern[12] = {{0, 7}, {1, 4}, {1, 4}, {0, 7}, {0, 7}, {1, 4}, {1, 4}, {0, 7}, {0, 7}, {1, 4}, {1, 5}, {1, 4}};
                        openGuitarQuest(pattern, 12, 60.0f);
                    } else openGuitarFreePlay();
                }
            }
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Tab && !isGuitarOpen()) openGuitarFreePlay();
                if (event.key.code == sf::Keyboard::R && isGuitarOpen()) closeGuitar();
            }
        } // End of PollEvent

        // ════════════════════════════════════════════════════════════════
        // 2. UPDATE LOGIC
        // ════════════════════════════════════════════════════════════════
        if (gState.currentState == STATE_MENU) MenuUpdate(window, gState.currentState);
        else if (gState.currentState == STATE_SETTINGS) SettingsUpdate(window, gState.currentState);
        else if (gState.currentState == STATE_PLAYING) {
            if (isGuitarOpen()) updateGuitar(gState.deltaTime);
            updateCutscene(gState.deltaTime);

            currentMap = worldGetCurrentMap(world);
            if (!currentMap) break;

            mainView = updateMapView(mainView, *currentMap, player.pos, gState.deltaTime);
            gameLogic.update(window, gState.currentState);
            updateDialogue(gState.deltaTime);
            inv.invt_update(window, gState.currentState, player.pos, gState.deltaTime);

            if (!gameLogic.isPaused && !isDialogueActive() && !isCutsceneActive() && !isGuitarOpen() && !isMinigameActive) {
                updatePlayer(gState.deltaTime, world);
                updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                //
                // updateWeapon(gState.deltaTime);
                //
                //
                updateEnemies(gState.deltaTime);
                updateChest(gState.deltaTime, world.currentMapName);
                updateBoss(gState.deltaTime);
                // Update block — add next to updateBoss:
                updateRounds(gState.deltaTime);
                updateEnemies(gState.deltaTime);
                updateFireballs(gState.deltaTime);

                // Portals Logic
                for (auto& p : currentMap->portals) {
                    if (sf::FloatRect(player.pos.x, player.pos.y, 48, 48).intersects(p.bounds)) {
                        worldSetCurrentMap(world, p.targetMap);
                        currentMap = worldGetCurrentMap(world);
                        player.pos = Vector2f(p.spawnPos.x * currentMap->tileSize, p.spawnPos.y * currentMap->tileSize);
                        fadeAlpha = 255.0f; isFading = true;
                        break;
                    }
                }
            }
        }

        if (isFading) {
            fadeAlpha -= fadeSpeed * gState.deltaTime;
            if (fadeAlpha <= 0) { fadeAlpha = 0; isFading = false; }
        }

        // ════════════════════════════════════════════════════════════════
        // 3. DRAW LOGIC
        // ════════════════════════════════════════════════════════════════
        window.clear();

        if (gState.currentState == STATE_MENU) MenuDraw(window, gState.currentState);
        else if (gState.currentState == STATE_SETTINGS) settings.draw(window);
        else if (gState.currentState == STATE_PLAYING) {
            window.setView(mainView);
            drawMap(window, *currentMap);
            drawNPCs(window, world.currentMapName, world.phaseSys.currentPhaseIdx);
            drawChest(window, world.currentMapName);
            drawStrings(window, world.phaseSys, world.currentMapName);
            drawEnemy(window);
            drawBoss(window);
            drawFireballs(window);
            drawPlayer(window);
            drawWeapons(window);

            window.setView(window.getDefaultView());

            // Interaction Prompt
            bool nearNPC = getNearbyNPCName(player.pos, world.currentMapName) != "";
            if (nearNPC && !isDialogueActive()) {
                window.draw(interactBoxSprite);
                window.draw(interactPrompt);
            }

            if (isDialogueActive()) drawDialogue(window);
            else inv.invt_draw(window);

            drawHealthBar(window);
            drawXPBar(window);
            if (isGuitarOpen()) drawGuitar(window);
            drawCutsceneOverlay(window, font);

            if (inv.feedbackTimer > 0) {
                window.draw(inv.feedbackSprite);
                for (int i = 0; i < 5; i++) window.draw(inv.sparkles[i]);
            }
            window.setView(window.getDefaultView());
            drawWeapons(window);
            bool nearNPC    = getNearbyNPCName(player.pos, world.currentMapName) != "";
            bool nearChest  = !gameChest.isOpen &&
                              std::sqrt(std::pow(player.pos.x - gameChest.pos.x, 2) +
                                        std::pow(player.pos.y - gameChest.pos.y, 2)) < 80.f &&
                              gameChest.mapName == world.currentMapName;
            bool nearString = canPickupString(world.phaseSys, player.pos, world.currentMapName, world.hintSys);;

            if ((nearNPC || nearChest || nearString) && !isDialogueActive()) {
                // غير النص حسب السياق
                if (nearString && !nearNPC && !nearChest)
                    interactPrompt.setString("Press E to pickup");
                else
                    interactPrompt.setString("Press E to interact");

                window.draw(interactBoxSprite);

                sf::FloatRect boxBounds  = interactBoxSprite.getGlobalBounds();
                sf::FloatRect textBounds = interactPrompt.getLocalBounds();
                interactPrompt.setPosition(
                    boxBounds.left + (boxBounds.width  - textBounds.width)  / 2.f - textBounds.left,
                    boxBounds.top  + (boxBounds.height - textBounds.height) / 2.f - textBounds.top - 5.f
                );
                window.draw(interactPrompt);
            }

            if (isDialogueActive()) {
                drawDialogue(window);
            } else {
                inv.invt_draw(window);
            }

            drawHealthBar(window);
            drawHintIcon(window, world.hintSys);
            drawHintPage(window, world.hintSys);

            if (warningTimer > 0) {
                sf::Text popUp;
                popUp.setFont(font);
                popUp.setString(warningMessage.getString());
                popUp.setCharacterSize(24);
                popUp.setFillColor(sf::Color::Red);
                popUp.setOutlineColor(sf::Color::Black);
                popUp.setOutlineThickness(2);
                popUp.setPosition(SCREEN_W / 2.0f - popUp.getGlobalBounds().width / 2.0f, 200.f);
                window.draw(popUp);
                warningTimer -= gState.deltaTime;
            }

            drawXPBar(window);
            gameLogic.draw(window);

            if (isGuitarOpen()) {
                window.setView(window.getDefaultView());
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

        if (!isGuitarOpen()) {
            window.draw(statusTrackerText);
        }

        // Final Overlays
        if (fadeAlpha > 0) {
            sf::RectangleShape fO(sf::Vector2f(SCREEN_W, SCREEN_H));
            fO.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
            window.draw(fO);
        }

        if (isMinigameActive) {
            sf::RectangleShape overlay(sf::Vector2f(SCREEN_W, SCREEN_H));
            overlay.setFillColor(sf::Color(0, 0, 0, 230));
            window.draw(overlay);
            drawReviewGame(window, terminalSprite, terminalFont, myReview);
        }

        window.display();
    }
    return 0;
}