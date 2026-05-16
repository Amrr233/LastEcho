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
#include "BTDminigame.h"
using namespace sf;
using namespace std;

RenderWindow window;
GameState    gState;
Player       player;
World        world;
inventory    inv;
AppState     last_state;
float        warningTimer = 0.0f;
float        fadeAlpha    = 255.0f;
float        fadeSpeed    = 180.0f;
float spawnX = 350;
float spawnY = 900;
bool         isFading     = true;
bool  bossLostScreen  = false;
Text         warningMessage;
Text         statusTrackerText;
Font         font;
Texture  interactBoxTex;
Sprite   interactBoxSprite;
Text     interactPrompt;
extern GuitarGame g_guitar;
float lostScreenTimer = 0.f;
float blurAlpha       = 0.f;
sf::RectangleShape blurOverlay;
sf::Font lostFont;
sf::Text lostText;





int main() {


    window.create(VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf"))
        cout << "ERROR: Font not found!" << endl;

    if (!worldLoadAllMaps(world)){
        cout << "CRITICAL ERROR: Failed to load world!" << endl;
        return -1;
    }
    GameMap* currentMap = worldGetCurrentMap(world);
    if (!currentMap) return -1;

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

    blurOverlay.setSize(sf::Vector2f(SCREEN_W, SCREEN_H));
    blurOverlay.setFillColor(sf::Color(0, 0, 0, 0));
    lostFont.loadFromFile("assets/fonts/pixelsix00.ttf");
    lostText.setFont(lostFont);
    lostText.setString("YOU LOST");
    lostText.setCharacterSize(72);
    lostText.setFillColor(sf::Color(255, 0, 0, 0));
    lostText.setStyle(sf::Text::Bold);
    sf::FloatRect lostBounds = lostText.getLocalBounds();
    lostText.setOrigin(lostBounds.width / 2.f, lostBounds.height / 2.f);
    lostText.setPosition(SCREEN_W / 2.f, SCREEN_H / 2.f);

    initCutsceneSystem();
    phaseInit(world.phaseSys);
    initHintSystem(world.hintSys);
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initBoss();
    initNPCs(world);
    initChest(Vector2f(100.f, 150.f), "sclab");
    initGuitar();
    initDialogue();
    BinaryGameData myBinaryGame;
    initBinaryGame(myBinaryGame);
    myBinaryGame.targetBinary = "1010";
    myBinaryGame.targetDecimal = 10;
    myBinaryGame.active = false;


    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    //gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
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

    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();

        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Z) {
                if (isMinigameActive)
                    isMinigameActive = false;
                else if (gState.currentState == STATE_PLAYING && !isDialogueActive() && !isCutsceneActive() && !isGuitarOpen())
                    isMinigameActive = true;
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::H) {
                if (myBinaryGame.active) {
                    myBinaryGame.active = false;
                } else if (gState.currentState == STATE_PLAYING && !isDialogueActive() && !isCutsceneActive() && !isGuitarOpen() && !isMinigameActive) {
                    restartBinaryGame(myBinaryGame);
                    myBinaryGame.active = true;
                }
                continue;
            }
            if (myBinaryGame.active) {
                handleBinaryInput(myBinaryGame, event);
                continue;
            }
            if (myBinaryGame.active) {
                handleBinaryInput(myBinaryGame, event);
                if (event.type == Event::KeyPressed && (event.key.code == Keyboard::M || event.key.code == Keyboard::H)) {
                    myBinaryGame.active = false;
                }
                continue;
            }

            if (isMinigameActive) {
                updateReviewInput(event, myReview);
                continue;
            }

            if (gState.currentState == STATE_PLAYING) {
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                    if (isDialogueActive()) {
                        nextLine();
                    } else if (!isCutsceneActive() && !isGuitarOpen()) {
                        if (tryOpenChest(player.pos, world.currentMapName)) {
                        } else if (canPickupString(world.phaseSys, player.pos, world.currentMapName, world.hintSys)) {
                            pickupString(world.phaseSys, player.pos, world.currentMapName, world.hintSys);
                        } else {
                            string npcName = getNearbyNPCName(player.pos, world.currentMapName);
                            if (npcName != "")
                                updatePhaseLogic(world.phaseSys, npcName, world.hintSys);
                        }
                    }
                }

                if (isGuitarOpen()) {
                    if (event.type == Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                        handleGuitarClick(window, sf::Mouse::getPosition(window));
                    if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
                        if (g_guitar.mode == GUITAR_FREE) {
                            GuitarNote pattern[12] = {
                                {0,7},{1,4},{1,4},{0,7},
                                {0,7},{1,4},{1,4},{0,7},
                                {0,7},{1,4},{1,5},{1,4}
                            };
                            openGuitarQuest(pattern, 12, 60.0f);
                        } else {
                            openGuitarFreePlay();
                        }
                    }
                }

                if (event.type == Event::KeyPressed) {
                    if (event.key.code == Keyboard::Tab && !isGuitarOpen())
                        openGuitarFreePlay();
                    if (event.key.code == sf::Keyboard::R && isGuitarOpen())
                        closeGuitar();
                    if (event.key.code == sf::Keyboard::Q && !isGuitarOpen()) {
                        if (world.hintSys.isOpen) {
                            world.hintSys.isOpen     = false;
                            world.hintSys.hasNewHint = false;
                        } else if (world.hintSys.hintsUnlocked > 0) {
                            world.hintSys.isOpen = true;
                        }
                    }
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::X && isCutsceneActive())
                        stopCutscene();
                }
            }

            if (gState.currentState == STATE_MENU)
                MenuUpdate(window, gState.currentState);
            else if (gState.currentState == STATE_SETTINGS)
                SettingsUpdate(window, gState.currentState);
        }

        if (gState.currentState == STATE_PLAYING) {
            if (isGuitarOpen())
                updateGuitar(gState.deltaTime);
            if (myBinaryGame.active)
                updateBinaryGame(myBinaryGame, gState.deltaTime);
            updateCutscene(gState.deltaTime);
            triggerPickSound(world.phaseSys);

            Phase& cp = world.phaseSys.allPhases[world.phaseSys.currentPhaseIdx];
            Quest& cq = cp.quests[cp.currentQuestIdx];
            statusTrackerText.setString("Phase: " + cp.phaseTitle + "\n" + "Quest: " + cq.title);

            currentMap = worldGetCurrentMap(world);

            if (currentMap) {
                mainView = updateMapView(mainView, *currentMap, player.pos, gState.deltaTime);
                //gameLogic.update(window, gState.currentState);
                updateDialogue(gState.deltaTime);
                inv.invt_update(window, gState.currentState, player.pos, gState.deltaTime);

                if (/*!gameLogic.isPaused &&*/ !isDialogueActive() && !isCutsceneActive() && !isGuitarOpen() && !isMinigameActive && !myBinaryGame.active && !bossLostScreen) {
                    updatePlayer(gState.deltaTime, world);
                    updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                    updateEnemies(gState.deltaTime);
                    updateChest(gState.deltaTime, world.currentMapName);
                    if (boss.isActive) {
                        updateBoss(gState.deltaTime);
                        updateRounds(gState.deltaTime);
                        updateFireballs(gState.deltaTime);
                    }

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

                if (roundMan.playerDied && !bossLostScreen) {
                    bossLostScreen  = true;
                    lostScreenTimer = 0.f;
                    blurAlpha       = 0.f;
                    roundMan.playerDied = false;
                }

                if (bossLostScreen) {
                    lostScreenTimer += gState.deltaTime;

                    blurAlpha = std::min(200.f, blurAlpha + 200.f * gState.deltaTime);
                    blurOverlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)blurAlpha));

                    if (lostScreenTimer >= 3.f) {
                        bossLostScreen  = false;
                        player.hp       = player.maxHp;
                        player.currentState = IDLE;
                        player.isInvincible = false;
                        player.hurt_timer   = 0.f;
                        startRound(roundMan.currentRound);
                    }
                }
            }
        }

        if (isFading) {
            fadeAlpha -= fadeSpeed * gState.deltaTime;
            if (fadeAlpha <= 0) { fadeAlpha = 0; isFading = false; }
        }

        window.clear();

        if (gState.currentState == STATE_MENU)
            MenuDraw(window, gState.currentState);
        else if (gState.currentState == STATE_SETTINGS)
            settings.draw(window);
        else if (gState.currentState == STATE_PLAYING) {
            window.setView(mainView);
            drawMap(window, *currentMap);
            drawNPCs(window, world.currentMapName, world.phaseSys.currentPhaseIdx);
            drawChest(window, world.currentMapName);
            drawStrings(window, world.phaseSys, world.currentMapName);
            // drawEnemy(window);
            drawPlayer(window);
            if (boss.isActive) {
                drawBoss(window);
                drawFireballs(window);
            }
            drawCutsceneOverlay(window, font);


            window.setView(window.getDefaultView());
            bool nearNPC    = getNearbyNPCName(player.pos, world.currentMapName) != "";
            bool nearChest  = !gameChest.isOpen &&
                (sqrt(pow(player.pos.x - gameChest.pos.x, 2) + pow(player.pos.y - gameChest.pos.y, 2)) < 80.f) &&
                (gameChest.mapName == world.currentMapName);
            bool nearString = canPickupString(world.phaseSys, player.pos, world.currentMapName, world.hintSys);

            if ((nearNPC || nearChest || nearString) && !isDialogueActive() && !isCutsceneActive() && !isMinigameActive && !myBinaryGame.active) {
                if (nearString && !nearNPC && !nearChest)
                    interactPrompt.setString("Press E to pickup");
                else
                    interactPrompt.setString("Press E to interact");
                window.draw(interactBoxSprite);
                FloatRect boxBounds  = interactBoxSprite.getGlobalBounds();
                FloatRect textBounds = interactPrompt.getLocalBounds();
                interactPrompt.setPosition(
                    boxBounds.left + (boxBounds.width  - textBounds.width)  / 2.f,
                    boxBounds.top  + (boxBounds.height - textBounds.height) / 2.f - 5.f);
                window.draw(interactPrompt);
            }

            if (isDialogueActive())
                drawDialogue(window);
            else
                inv.invt_draw(window);

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


            if (isGuitarOpen())
                drawGuitar(window);

            if (bossLostScreen) {
                window.setView(window.getDefaultView());
                window.draw(blurOverlay);

                if (lostScreenTimer >= 0.5f) {
                    sf::Uint8 alpha = (sf::Uint8)std::min(255.f, (lostScreenTimer - 0.5f) * 200.f);
                    lostText.setFillColor(sf::Color(255, 0, 0, alpha));
                    window.draw(lostText);
                }
            }
        }

        if (isMinigameActive) {
            RectangleShape overlay(Vector2f(SCREEN_W, SCREEN_H));
            overlay.setFillColor(Color(0, 0, 0, 230));
            window.draw(overlay);
            drawReviewGame(window, terminalSprite, terminalFont, myReview);
        }

        if (myBinaryGame.active) {
            window.setView(window.getDefaultView()); // تأكيد إن الرسم واخد أبعاد الشاشة الثابتة
            drawBinaryGame(window, myBinaryGame);
        }

        statusTrackerText.setFont(font);
        statusTrackerText.setCharacterSize(20);
        statusTrackerText.setFillColor(sf::Color::White);
        statusTrackerText.setOutlineColor(sf::Color::Black);
        statusTrackerText.setOutlineThickness(2);
        statusTrackerText.setPosition(20.f, 140.f);
        if (!isGuitarOpen())
            window.draw(statusTrackerText);

        if (fadeAlpha > 0) {
            sf::RectangleShape fO(sf::Vector2f(SCREEN_W, SCREEN_H));
            fO.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
            window.draw(fO);
        }

        window.display();
    }

    return 0;
}