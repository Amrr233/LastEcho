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
#include "enemies.h"
#include "DialogueManager.h"
#include "phase.h"
#include "BTDminigame.h"

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
BinaryGameData myBinaryGame; // بنعرف الـ Data





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


    initBinaryGame(myBinaryGame);


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
    // هيعمل سبون قدام السلم
    phaseInit(world.phaseSys);
    initPlayer(Vector2f(spawnX, spawnY));
    initEnemy(0, sf::Vector2f(spawnX + 100.f, spawnY + 100.f), BASIC_ENEMY);
    initNPCs(world);
    initweapon(Vector2f(spawnX, spawnY));

    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inv.invt_init((float)SCREEN_W, (float)SCREEN_H);
    initNPCs(world);
    initDialogue();

    Clock clock;
    // audio.playBGM();

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
                handleBinaryInput(myBinaryGame, event);
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::M) {
                    myBinaryGame.active = !myBinaryGame.active; // بيفتح ويقفل بـ M
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                    if (isDialogueActive()) {
                        nextLine(); // لو فيه حوار، قلب الصفحة
                    }
                    else {
                        // ابحث عن NPC قريب (هتستخدم دالة التفاعل بتاعتك)
                        string npcName = getNearbyNPCName(player.pos, world.currentMapName);

                        if (npcName != "") {
                            // 3. ابعت الاسم لـ Phase.cpp وهو هيقرر يفتح أنهي ديالوج بناءً على الـ Flags
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

            updateBinaryGame(myBinaryGame, gState.deltaTime);

            statusTrackerText.setString(
    "Phase: " + cp.phaseTitle + "\n" +
    "Quest: " + cq.title
);
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
            updateDialogue(gState.deltaTime); // 🔥 تم التعديل (دالة عادية)
            inv.invt_update(window, gState.currentState, player.pos, gState.deltaTime);

            if (!gameLogic.isPaused && !isDialogueActive()) { // 🔥 تم التعديل (دالة عادية)
                updatePlayer(gState.deltaTime, world);
                checkDialogueReward(world.phaseSys);

                // NPC update - uses current map name from World
                updateNPCs(gState.deltaTime, world.currentMapName, player.pos);
                updateWeapon(gState.deltaTime);

                updateEnemies(gState.deltaTime);

                for (auto& p : currentMap->portals) {
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);

                    if (playerBounds.intersects(p.bounds)) {
                        // تشيك: لو بيحاول يدخل وهو لسه مخلصش المهمة
                        if (p.targetMap == "lobby" && world.phaseSys.currentPhaseIdx == 0) {

                            // 🔥 التعديل هنا: بدل الديالوج، هنظهر رسالة Pop-up سريعة
                            // أو نغير قيمة الـ String في نص الشاشة
                            warningMessage.setString("The gate is locked. Talk to the security guard!") ;
                            warningTimer = 1.0f; // الرسالة هتفضل ظاهرة لثانيتين

                            // ممكن تعمل صوت "تيت" بسيط هنا برضه
                            // audio.playErrorSound();

                            break; // اخرج من اللوب وما تنقلش الماب
                        }

                        worldSetCurrentMap(world, p.targetMap);
                        currentMap = worldGetCurrentMap(world);

                        player.pos.x = p.spawnPos.x * currentMap->tileSize;
                        player.pos.y = p.spawnPos.y * currentMap->tileSize;
                        player.sprite.setPosition(player.pos);

                        fadeAlpha = 255.0f;
                        isFading = true;

                        std::cout << "[PORTAL] Moved to " << p.targetMap
                                  << " at " << player.pos.x << "," << player.pos.y << std::endl;
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

            // Draw NPCs - uses current map name from World
            drawNPCs(window, world.currentMapName, world.phaseSys.currentPhaseIdx);
            drawEnemy(window);
            drawPlayer(window);
            if (inv.feedbackTimer > 0) {
                window.draw(inv.feedbackSprite);
                for (int i = 0; i < 5; i++) {
                    window.draw(inv.sparkles[i]);
                }
            }
            drawWeapons(window);
            for (auto& p : currentMap->portals) {
                sf::RectangleShape debugRect(sf::Vector2f(p.bounds.width, p.bounds.height));
                debugRect.setPosition(p.bounds.left, p.bounds.top);
                debugRect.setFillColor(Color(0, 0, 0, 75));
                window.draw(debugRect);
            }
            window.setView(window.getDefaultView());
            drawBinaryGame(window, myBinaryGame);

            if (isDialogueActive()) {
                drawDialogue(window);
            }
            else {
                inv.invt_draw(window);
            }
            if (myBinaryGame.active) {
                drawBinaryGame(window, myBinaryGame);
            }


            if (warningTimer > 0) {
                sf::Text popUp;
                popUp.setFont(font); // استخدم الفونت بتاعك
                popUp.setString("The gate is locked. Talk to the security guard!");
                popUp.setCharacterSize(24);
                popUp.setFillColor(sf::Color::Red);
                popUp.setOutlineColor(sf::Color::Black);
                popUp.setOutlineThickness(2);

                // سنتر الرسالة في نص الشاشة فوق شوية
                popUp.setPosition(SCREEN_W/2.0f - popUp.getGlobalBounds().width/2.0f, 200.f);

                window.draw(popUp);

                // نقص التايمر عشان تختفي
                warningTimer -= gState.deltaTime;
            }
            if (!myBinaryGame.active) {
                window.draw(statusTrackerText);
            }
            
            gameLogic.draw(window);
        }

        // تأكد إنك فاتح الـ DefaultView عشان التيكست ميمشيش مع الكاميرا
        window.setView(window.getDefaultView());

        statusTrackerText.setFont(font); // استخدم الفونت بتاعك
        statusTrackerText.setCharacterSize(20);
        statusTrackerText.setFillColor(sf::Color::White); // لون رايق يبان على أي خلفية
        statusTrackerText.setOutlineColor(sf::Color::Black);
        statusTrackerText.setOutlineThickness(2);
        statusTrackerText.setPosition(20.f, 140.f);
        if (!myBinaryGame.active) {
            window.draw(statusTrackerText);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) healing(10);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) damaging(10);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            inv.triggerPickupEffect("assets/items/idcard.png");
            inv.addItem("id_card", "assets/items/idcard.png");;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)) {
            inv.triggerPickupEffect("assets/items/sword.png");
            inv.addItem("sword", "assets/items/sword.png");;
        }

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
