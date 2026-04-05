#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"
#include "MainMenu.h"
#include <CameraAndPortal.h>

// ← أضف

sf::RenderWindow window;
GameState        gState;
Player           player;

bool isSolid(float x, float y) { return false; }
bool mapLoaded = false;

int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "يارب نخلص");
    window.setFramerateLimit(60);

    gState.currentState = STATE_MENU;      // ← غيّر لـ MENU
    gState.currentPhase = 0;

    MenuStart(window);                     // ← أضف
    initPlayer(sf::Vector2f(SCREEN_W / 2, SCREEN_H / 2));

    sf::Clock clock;

    while (window.isOpen()) {

        gState.deltaTime = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        // UPDATE
        MenuUpdate(window, gState.currentState);
        if (gState.currentState == STATE_PLAYING && !mapLoaded) {
            loadMap(
                "assets/maps/outside/outside.png",
                {
                    "assets/maps/outside/_Ground.csv",
                    "assets/maps/outside/_Staircase.csv",
                    "assets/maps/outside/_Others.csv"
                },
                60, 70, 16   // widthTiles, heightTiles, tileSize
            );
            mapLoaded = true;
        }

        // RENDER
        window.clear(sf::Color::Black);
        MenuDraw(window, gState.currentState);
        if (gState.currentState == STATE_PLAYING)
            updatePlayer(gState.deltaTime);
        window.display();
    }

    return 0;
}