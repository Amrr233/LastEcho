#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "settings.h"
#include "GameMap.h"
#include "audio.h"
#include "Game.h"
#include <iostream>
#include "healthbar.h"
#include "inventory.h"
#include "XPBar.h"

using namespace sf;
using namespace std;

// ==============================
// GLOBALS
// ==============================
RenderWindow window;
GameState    gState;
Player       player;
AudioManager audio;
GameMap      myMap;
Game         gameLogic;
inventory    inventory;

int main() {
    // 1. إنشاء النافذة
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    // 2. تحميل الماب الافتراضية (البداية)
    if (!loadMapFromJSON(myMap, "assets/maps/outside/outside.json")) {
        cout << "CRITICAL ERROR: Failed to load map file!" << endl;
        return -1;
    }

    // احداثيات الهتعمل فيها spawn
    float spawnX = (float)(myMap.width * myMap.tileSize) / 2.0f;
    float spawnY = (float)(myMap.height * myMap.tileSize) / 2.0f;
    initPlayer(Vector2f(spawnX, spawnY + 80));

    gState.currentState = STATE_MENU;
    MenuStart(window);
    settings.init(SCREEN_W, SCREEN_H);
    gameLogic.init((float)SCREEN_W, (float)SCREEN_H);
    inventory.invt_init((float)SCREEN_W, (float)SCREEN_H);

    Clock clock;
    audio.playBGM();

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
        }
        // --- UPDATE LOGIC ---
        if (gState.currentState == STATE_PLAYING) {

            gameLogic.update(window, gState.currentState);
            // tool bar
            inventory.invt_update(window, gState.currentState);

            if (!gameLogic.isPaused) {
                updatePlayer(gState.deltaTime);
                // ========================================================
                // سيستم الانتقال بين المابس وحركات وبركات
                for (auto& p : myMap.portals) {
                    // هنعمل مربع يدوي للاعب بناءً على مكانه الحالي ومقاسه (48x48)
                    sf::FloatRect playerBounds(player.pos.x, player.pos.y, 48.f, 48.f);

                    // بنتشيك هل مربع اللاعب بيخبط في مربع البوابة
                    if (playerBounds.intersects(p.bounds)) {
                        string fullPath = "assets/maps/" + p.targetMap + "/" + p.targetMap + ".json";

                        if (loadMapFromJSON(myMap, fullPath)) {
                            // بنضرب القيمة اللي جاية من JSON في حجم التايلة
                            player.pos.x = p.spawnPos.x * myMap.tileSize;
                            player.pos.y = p.spawnPos.y * myMap.tileSize;

                            player.sprite.setPosition(player.pos);

                            std::cout << "[Final Fix]: Player moved to Pixel (" << player.pos.x << "," << player.pos.y << ")" << std::endl;
                            break;
                        }
                    }
                }
                // ========================================================
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
            // تظبيط الكاميرا على الماب الجديدة
            window.setView(getMapView(myMap));

            drawMap(window, myMap);
            drawPlayer(window);

            // رسم مربعات البوابات للتأكد من مكانها (Debug)
            for (auto& p : myMap.portals) {
                sf::RectangleShape debugRect(sf::Vector2f(p.bounds.width, p.bounds.height));
                debugRect.setPosition(p.bounds.left, p.bounds.top);
                debugRect.setFillColor(sf::Color(255, 0, 0, 100));
                window.draw(debugRect);
            }

            // العودة للـ View الافتراضية لرسم الـ UI فوق الماب
            window.setView(window.getDefaultView());
            inventory.invt_draw(window);
            drawHealthBar(window);
            drawXPBar(window);
            gameLogic.draw(window);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) healing(10);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) damaging(10);

        window.display();
    }

    return 0;
}