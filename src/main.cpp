// include everything, every feature
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

//Global Variables
RenderWindow window;
GameState    gState;
Player       player;
World        world;
Game         gameLogic;
inventory    inv;
AppState     last_state;
float        warningTimer = 0.0f;
float        fadeAlpha    = 255.0f;
float        fadeSpeed    = 180.0f;
float spawnX = 350;
float spawnY = 900;
bool         isFading     = true;
Text         warningMessage;
Text         statusTrackerText;
Font         font;
Texture  interactBoxTex;
Sprite   interactBoxSprite;
Text     interactPrompt;
extern AudioManager audioManager;
extern GuitarGame g_guitar;

int main() {
    window.create(VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);
    //load the font
    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf"))
        cout << "ERROR: Font not found!" << endl;


    //load the current map
    if (!worldLoadAllMaps(world)) {
        cout << "CRITICAL ERROR: Failed to load world!" << endl;
        return -1;
    }
    GameMap* currentMap = worldGetCurrentMap(world);
    if (!currentMap) return -1;


    //load and set up the press e to interact box
    interactBoxTex.loadFromFile("assets/sprites/items/Text_Box.png");
    interactBoxSprite.setTexture(interactBoxTex);
    interactBoxSprite.setScale(0.35f, 0.35f);
    float boxW = interactBoxSprite.getGlobalBounds().width;
    float boxH = interactBoxSprite.getGlobalBounds().height;
    interactBoxSprite.setPosition(SCREEN_W - boxW - 170.f, SCREEN_H - boxH - 20.f);
    interactPrompt.setFont(font);
    interactPrompt.setCharacterSize(16);
    interactPrompt.setFillColor(sf::Color(60, 30, 10));
    interactPrompt.setOutlineColor(sf::Color::Black);
    interactPrompt.setOutlineThickness(1);


    // --- Systems Initialization ---
    initCutsceneSystem();
    phaseInit(world.phaseSys);
    initHintSystem(world.hintSys);
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0,Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initBoss();
    initNPCs(world);
    initChest(Vector2f(100.f, 150.f), "sclab");
    initweapon(Vector2f(spawnX, spawnY));
    initGuitar();
    initDialogue();


    // when you first open the game your state will be menu (the main menu)
    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inv.invt_init((float)SCREEN_W, (float)SCREEN_H);

    MovieReview myReview;
    initReviewGame(myReview);

    sf::Texture terminalTex;
    terminalTex.loadFromFile("Assets/gameplay/terminalasset.png");
    sf::Sprite terminalSprite(terminalTex);
    terminalSprite.setScale(0.7f, 0.7f);
    terminalSprite.setOrigin(
        terminalSprite.getLocalBounds().width  / 2.f,
        terminalSprite.getLocalBounds().height / 2.f);
    terminalSprite.setPosition(SCREEN_W / 2.f, SCREEN_H / 2.f);

    sf::Font terminalFont;
    terminalFont.loadFromFile("Assets/fonts/pixelsix00.ttf");

    bool isMinigameActive = false;

    Clock clock;
    View  mainView;
    mainView.setSize(SCREEN_W, SCREEN_H);

    // ════════════════════════════════════════════════════════════
    // GAME LOOP
    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();
        // ════════════════════════════════════════════════════════
        // Events LOGIC
        Event event;
        //go look if there is any event happened in the current frame return true if you found false if not
        while (window.pollEvent(event)) {
            //if some line of code closed the game its considered an event
            if (event.type == Event::Closed) window.close();

            // if the user pressed Z open HiddenWordMiniGame
            if (event.type == Event::KeyPressed &&event.key.code == Keyboard::Z)
            {
                if (isMinigameActive) {
                    isMinigameActive = false;
                }
                else if (gState.currentState == STATE_PLAYING &&!isDialogueActive() && !isCutsceneActive() && !isGuitarOpen()){
                    isMinigameActive = true;
                }
            }

            // if the minigame IS active send him the input
            if (isMinigameActive) {
                updateReviewInput(event, myReview);
                continue;
            }

            // if the state is playing look for other events
            if (gState.currentState == STATE_PLAYING) {

                // If the user pressed E : (talking / interacting)
                if (event.type == Event::KeyPressed &&event.key.code == Keyboard::E)
                {
                    if (isDialogueActive()) {
                        nextLine();
                    }
                    else if (!isCutsceneActive() && !isGuitarOpen()) {
                        if (tryOpenChest(player.pos, world.currentMapName) ) {

                        }
                        else if (canPickupString(world.phaseSys, player.pos,world.currentMapName, world.hintSys)) {
                            pickupString(world.phaseSys, player.pos,world.currentMapName, world.hintSys);
                        }
                        else {
                            string npcName = getNearbyNPCName(player.pos, world.currentMapName);
                            if (npcName != "")
                                updatePhaseLogic(world.phaseSys, npcName, world.hintSys);
                        }
                    }
                }

                // Guitar controls
                if (isGuitarOpen()) {
                    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                        handleGuitarClick(window, sf::Mouse::getPosition(window));
                    }
                    if (event.type == Event::KeyPressed &&event.key.code == sf::Keyboard::Q)
                    {
                        if (g_guitar.mode == GUITAR_FREE) {
                            GuitarNote pattern[12] = {
                                {0,7},{1,4},{1,4},{0,7},
                                {0,7},{1,4},{1,4},{0,7},
                                {0,7},{1,4},{1,5},{1,4}
                            };
                            openGuitarQuest(pattern, 12, 60.0f);
                        }
                        else {
                            openGuitarFreePlay();
                        }
                    }
                }

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Tab && !isGuitarOpen() && inv.itemNames[inv.selectedSlot] == "magical_guitar")
                        openGuitarFreePlay();
                    if (event.key.code == sf::Keyboard::R && isGuitarOpen())
                        closeGuitar();

                    // Q: hint system
                    if (event.key.code == sf::Keyboard::Q && !isGuitarOpen()) {
                        if (world.hintSys.isOpen) {
                            world.hintSys.isOpen     = false;
                            world.hintSys.hasNewHint = false;
                        } else if (world.hintSys.hintsUnlocked > 0) {
                            world.hintSys.isOpen = true;
                        }
                    }
                    if (event.type == Event::KeyPressed &&event.key.code == Keyboard::X &&isCutsceneActive())
                    {
                        stopCutscene();
                    }
                }
            }

            if (gState.currentState == STATE_MENU)
                MenuUpdate(window, gState.currentState);
            else if (gState.currentState == STATE_SETTINGS)
                SettingsUpdate(window, gState.currentState);
        }

        // ════════════════════════════════════════════════════════
        // UPDATE LOGIC


        if (gState.currentState == STATE_PLAYING) {
            if (isGuitarOpen()) {
                updateGuitar(gState.deltaTime);
            }
            updateCutscene(gState.deltaTime);

            // the tracker
            Phase& cp = world.phaseSys.allPhases[world.phaseSys.currentPhaseIdx];
            Quest& cq = cp.quests[cp.currentQuestIdx];
            statusTrackerText.setString("Phase: " + cp.phaseTitle + "\n" + "Quest: " + cq.title);


            // check what map are we in now
            currentMap = worldGetCurrentMap(world);

            if (currentMap) {
                //set up the camera
                mainView = updateMapView(mainView, *currentMap, player.pos, gState.deltaTime);
                gameLogic.update(window, gState.currentState);
                updateDialogue(gState.deltaTime);
                inv.invt_update(window, gState.currentState, player.pos, gState.deltaTime);

                // if we are not talking or in any minigame or in settings update
                if (!gameLogic.isPaused  &&!isDialogueActive()  &&!isCutsceneActive()  &&!isGuitarOpen()  &&!isMinigameActive){
                    updatePlayer(gState.deltaTime, world);
                    updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                    updateEnemies(gState.deltaTime);
                    updateChest(gState.deltaTime, world.currentMapName);
                    if (boss.isActive) {
                        updateBoss(gState.deltaTime);
                        updateRounds(gState.deltaTime);
                        updateFireballs(gState.deltaTime);
                    }
                    //loop over all the portals to see if the player intersect with any of them
                    for (auto& p : currentMap->portals) {
                        FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);
                        if (playerBounds.intersects(p.bounds)) {
                            if (p.targetMap == "lobby" && world.phaseSys.currentPhaseIdx == 0 && world.phaseSys.allPhases[0].currentQuestIdx < 2) {
                                warningMessage.setString("The gate is locked. Talk to the security guard!");
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
        }
        if (isFading) {
            fadeAlpha -= fadeSpeed * gState.deltaTime;
            if (fadeAlpha <= 0) { fadeAlpha = 0; isFading = false; }
        }

        // ════════════════════════════════════════════════════════
        // DRAW LOGIC
        window.clear();

        if (gState.currentState == STATE_MENU)
            MenuDraw(window, gState.currentState);
        else if (gState.currentState == STATE_SETTINGS)
            settings.draw(window);
        else if (gState.currentState == STATE_PLAYING) {
            // The camera view thing moves with it not static
            window.setView(mainView);
            drawMap(window, *currentMap);
            drawNPCs(window, world.currentMapName, world.phaseSys.currentPhaseIdx);
            drawChest(window, world.currentMapName);
            drawStrings(window, world.phaseSys, world.currentMapName);
            drawEnemy(window);
            drawPlayer(window);
            drawWeapons(window);
            if (boss.isActive) {
                drawBoss(window);
                drawFireballs(window);
            }


            // UI view (static)
            window.setView(window.getDefaultView());
            bool nearNPC = getNearbyNPCName(player.pos, world.currentMapName) != "";
            bool nearChest = !gameChest.isOpen &&
                ( sqrt(pow(player.pos.x - gameChest.pos.x, 2) + pow(player.pos.y - gameChest.pos.y, 2)) < 80.f) &&
                (gameChest.mapName == world.currentMapName);
            bool nearString = canPickupString(world.phaseSys, player.pos,world.currentMapName, world.hintSys);

            if ((nearNPC || nearChest || nearString) && !isDialogueActive() && !isCutsceneActive() && !isMinigameActive)
            {
                if (nearString && !nearNPC && !nearChest)
                    interactPrompt.setString("Press E to pickup");
                else
                    interactPrompt.setString("Press E to interact");
                window.draw(interactBoxSprite);
                FloatRect boxBounds  = interactBoxSprite.getGlobalBounds();
                FloatRect textBounds = interactPrompt.getLocalBounds();
                interactPrompt.setPosition( boxBounds.left + (boxBounds.width  - textBounds.width)  / 2.f, boxBounds.top  + (boxBounds.height - textBounds.height) / 2.f - 5.f);
                window.draw(interactPrompt);
            }

            if (isDialogueActive()) {
                drawDialogue(window);
            }
            else {
                inv.invt_draw(window);
            }

            drawHealthBar(window);
            drawXPBar(window);
            drawHintIcon(window, world.hintSys);
            drawHintPage(window, world.hintSys);
            if (warningTimer > 0) {
                Text popUp;
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
            drawCutsceneOverlay(window, font);
            gameLogic.draw(window);
            if (isGuitarOpen()) {
                drawGuitar(window);
            }
        }

        // ── Movie Review minigame overlay ────────────────────────
        if (isMinigameActive) {
            RectangleShape overlay(Vector2f(SCREEN_W, SCREEN_H));
            overlay.setFillColor(Color(0, 0, 0, 230));
            window.draw(overlay);
            drawReviewGame(window, terminalSprite, terminalFont, myReview);
        }

        statusTrackerText.setFont(font);
        statusTrackerText.setCharacterSize(20);
        statusTrackerText.setFillColor(sf::Color::White);
        statusTrackerText.setOutlineColor(sf::Color::Black);
        statusTrackerText.setOutlineThickness(2);
        statusTrackerText.setPosition(20.f, 140.f);
        if (!isGuitarOpen()) {
            window.draw(statusTrackerText);
        }

        // ── Fade overlay ─────────────────────────────────────────
        if (fadeAlpha > 0) {
            sf::RectangleShape fO(sf::Vector2f(SCREEN_W, SCREEN_H));
            fO.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
            window.draw(fO);
        }

        window.display();
    }

    return 0;
}