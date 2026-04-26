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

using namespace sf;
using namespace std;

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
bool        interactAssetsLoaded = false;
extern GuitarGame g_guitar;

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
    interactBoxTex.loadFromFile("assets/sprites/items/Text_Box.png");
    interactBoxSprite.setTexture(interactBoxTex);
    interactBoxSprite.setScale(0.35f, 0.35f); // عدل الحجم حسب ذوقك

    // تحت اليمين
    float boxW = interactBoxSprite.getGlobalBounds().width;
    float boxH = interactBoxSprite.getGlobalBounds().height;
    interactBoxSprite.setPosition(SCREEN_W - boxW - 170.f, SCREEN_H - boxH - 20.f);

    interactPrompt.setFont(font);
    interactPrompt.setString("Press E to interact");
    interactPrompt.setCharacterSize(16);
    interactPrompt.setFillColor(sf::Color(60, 30, 10)); // لون داكن يناسب الـ box
    interactPrompt.setOutlineColor(sf::Color::Black);
    interactPrompt.setOutlineThickness(1);

    float spawnX = 350;
    float spawnY = 900;

    initCutsceneSystem();
    phaseInit(world.phaseSys);
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initNPCs(world);
    initChest(sf::Vector2f(100.f, 150.f), "sclab"); // ← adjust position
    initweapon(Vector2f(spawnX, spawnY));
    initGuitar();

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
            // ════════════════════════════════════════════════════════════════
            // GUITAR CONTROLS
            // ════════════════════════════════════════════════════════════════
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                if (isGuitarOpen()) {
                    // بنبعت الـ window ومكان الماوس بالبكسل عشان الـ View تحولها صح
                    handleGuitarClick(window, sf::Mouse::getPosition(window));
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                // TAB - Open guitar
                if (event.key.code == sf::Keyboard::Tab && !isGuitarOpen()) {
                    openGuitarFreePlay();
                    // شيلنا دالة الـ Scale لأن الـ View الجديدة بتتكفل بكل حاجة
                }

                // ESC - Close guitar (or close game)
                if (event.key.code == sf::Keyboard::R) {
                    if (isGuitarOpen()) {
                        closeGuitar();
                    }
                }

                // Q - Switch mode (only when guitar open)
                if (isGuitarOpen() && event.key.code == sf::Keyboard::Q) {
                    if (g_guitar.mode == GUITAR_FREE) {
                        GuitarNote pattern[12] = {
                            {0, 7}, {1, 4}, {1, 4}, {0, 7},
                            {0, 7}, {1, 4}, {1, 4}, {0, 7},
                            {0, 7}, {1, 4}, {1, 5}, {1, 4}
                        };
                        openGuitarQuest(pattern, 12, 60.0f);
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
                if (!isCutsceneActive() && !isGuitarOpen()) {
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                        if (isDialogueActive()) {
                            nextLine();
                        }
                        else if (tryOpenChest(player.pos, world.currentMapName)) {
                            // chest opened, nothing extra needed
                        }
                        else {
                            string npcName = getNearbyNPCName(player.pos, world.currentMapName);
                            if (npcName != "") {
                                updatePhaseLogic(world.phaseSys, npcName);
                            }
                        }
                    }
                    //switching weapons
                    if (event.key.code == sf::Keyboard::F) {
                        weapon.switching(WEAPON_FIST);
                    }
                    if (event.key.code == sf::Keyboard::B) {
                        weapon.switching(WEAPON_BOOK);
                    }
                    if (event.key.code == sf::Keyboard::N) {
                        inv.triggerPickupEffect("assets/items/note.png");
                        inv.addItem("note","assets/items/note.png");
                    }
                } else {
                    if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                        if (isDialogueActive()) nextLine();
                    }
                }
            }
        }

        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {
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

            if (!gameLogic.isPaused && !isDialogueActive() && !isCutsceneActive() && !isGuitarOpen()) {
                updatePlayer(gState.deltaTime, world);
                checkDialogueReward(world.phaseSys);
                updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                updateWeapon(gState.deltaTime);
                updateEnemies(gState.deltaTime);
                updateChest(gState.deltaTime, world.currentMapName);

                for (auto& p : currentMap->portals) {
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);
                    if (playerBounds.intersects(p.bounds)) {
                        if (p.targetMap == "lobby" && world.phaseSys.currentPhaseIdx == 0 &&
                            world.phaseSys.allPhases[0].currentQuestIdx < 2) {
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
            drawChest(window, world.currentMapName);
            drawEnemy(window);
            drawPlayer(window);
            drawCutsceneOverlay(window, font);

            if (inv.feedbackTimer > 0) {
                window.draw(inv.feedbackSprite);
                for (int i = 0; i < 5; i++) window.draw(inv.sparkles[i]);
            }
            drawWeapons(window);

            window.setView(window.getDefaultView());
            bool nearNPC = getNearbyNPCName(player.pos, world.currentMapName) != "";
            bool nearChest = !gameChest.isOpen &&
                             std::sqrt(std::pow(player.pos.x - gameChest.pos.x, 2) +
                                       std::pow(player.pos.y - gameChest.pos.y, 2)) < 80.f &&
                             gameChest.mapName == world.currentMapName;

            if ((nearNPC || nearChest) && !isDialogueActive()) {
                // رسم الـ box
                window.draw(interactBoxSprite);

                // تسنيت النص في نص الـ box
                sf::FloatRect boxBounds = interactBoxSprite.getGlobalBounds();
                sf::FloatRect textBounds = interactPrompt.getLocalBounds();
                interactPrompt.setPosition(
                    boxBounds.left + (boxBounds.width  - textBounds.width)  / 2.f - textBounds.left,
                    boxBounds.top  + (boxBounds.height - textBounds.height) / 2.f - textBounds.top - 5.f
                );

                window.draw(interactPrompt);
            }

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

            // 🔥 Draw guitar on top
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

        if (fadeAlpha > 0) {
            sf::RectangleShape fadeOverlay(sf::Vector2f(SCREEN_W, SCREEN_H));
            fadeOverlay.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)fadeAlpha));
            window.draw(fadeOverlay);
        }

        window.display();
    }
    return 0;
}