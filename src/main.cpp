#include <SFML/Graphics.hpp>
#include "Data.h"
#include "player.h"

// ==============================
// GLOBALS — التعريف الفعلي هنا
// ==============================
sf::RenderWindow window;
GameState        gState;
Player           player;
TileMap          currentMap;

// ==============================
// isSolid — مؤقتة لحد ما نعمل map.cpp
// ==============================
bool isSolid(float x, float y) {
    return false; // مفيش collision دلوقتي
}

// ==============================
// MAIN
// ==============================
int main() {
    window.create(sf::VideoMode(SCREEN_W, SCREEN_H), "يارب نخلص");
    window.setFramerateLimit(60);

    // Init State
    gState.currentState = STATE_PLAYING;
    gState.currentPhase = 0;

    // Init Player — وسط الشاشة
    initPlayer(sf::Vector2f(SCREEN_W / 2, SCREEN_H / 2));

    sf::Clock clock;

    while (window.isOpen()) {

        gState.deltaTime = clock.restart().asSeconds();

        // 1. EVENTS
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();

        // 2. UPDATE
        updatePlayer(gState.deltaTime);

        // 3. RENDER
        window.clear(sf::Color::Black);
        drawPlayer(window);
        window.display();
    }

    return 0;
}