#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include "CameraAndPortal.h"
#include <cmath>
using namespace sf;

RenderWindow window;
GameState        gState;
Player           player;
bool mapLoaded = false;

int main() {

    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "The Last Echo of FCIS");
    window.setFramerateLimit(60);

    sf::RectangleShape mouseBox(sf::Vector2f(16, 16));
    mouseBox.setFillColor(sf::Color(255, 255, 255, 100));
    mouseBox.setOutlineThickness(1);

    gState.currentState = STATE_MENU;
    MenuStart(window);
    sf::Clock clock;

    while (window.isOpen()) {
        gState.deltaTime = clock.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            if (gState.currentState == STATE_PLAYING && event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                int gx = (int)mPos.x / currentMap.tileSize;
                int gy = (int)mPos.y / currentMap.tileSize;
                int idx = gy * currentMap.widthTiles + gx;

                if (idx >= 0 && idx < (int)currentMap.layers[0].tiles.size()) {
                    dugTiles.push_back(idx);
                }
            }
        }

        if (gState.currentState == STATE_PLAYING) {
            if (!mapLoaded) {
                // تقدر تبعت 2، 3، 5 أو حتى 10 ملفات براحتك
                loadMap("assets/maps/outside/outside.png",
                        {
                          "assets/maps/outside/_Ground.csv",
                          "assets/maps/outside/_Staircase.csv",
                          "assets/maps/outside/_Others.csv",
                          "assets/maps/outside/_Trees.csv" // مثال لطبقة رابعة زيادة
                        },
                        60, 16);

                initPlayer({480, 550});
                mapLoaded = true;
            }

            updatePlayer(gState.deltaTime);

        }

        window.clear();
        if (gState.currentState == STATE_MENU) {
            MenuUpdate(window, gState.currentState);
            MenuDraw(window, gState.currentState);
        } else {
            // تطبيق الـ View اللي بيعمل Stretch للماب على الشاشة
            sf::View cam = updateCamera(player, currentMap);
            window.setView(cam);

            drawMap(window);

            // مربع الماوس
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            mouseBox.setPosition(std::floor(mPos.x/16.f)*16.f, std::floor(mPos.y/16.f)*16.f);
            window.draw(mouseBox);

            drawPlayer(window);
        }
        window.display();
    }
    return 0;
}