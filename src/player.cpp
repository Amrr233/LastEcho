#include "player.h"

// ==============================
// EXTERN GLOBALS
// ==============================
extern Player           player;
extern TileMap          currentMap;

// ==============================
// LOCAL VARIABLES
// ==============================
static sf::RectangleShape playerRect;

// ==============================
// FUNCTIONS
// ==============================
void initPlayer(sf::Vector2f startPos) {
    player.pos      = startPos;
    player.velocity = sf::Vector2f(0, 0);
    player.speed    = 120.f;
    player.hp       = 100;
    player.maxHp    = 100;
    player.xp       = 0;
    player.level    = 1;
    player.isMoving = false;
    player.facing   = DIR_DOWN;

    playerRect.setSize(sf::Vector2f(TILE_SIZE - 4, TILE_SIZE - 4));
    playerRect.setFillColor(sf::Color::Green);
    playerRect.setOrigin(
        playerRect.getSize().x / 2,
        playerRect.getSize().y / 2
    );
}

void updatePlayer(float dt) {
    sf::Vector2f velocity(0, 0);

    // WASD Input
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        velocity.y = -player.speed;
        player.facing = DIR_UP;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        velocity.y = player.speed;
        player.facing = DIR_DOWN;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x = -player.speed;
        player.facing = DIR_LEFT;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x = player.speed;
        player.facing = DIR_RIGHT;
    }

    // Normalize diagonal movement
    if (velocity.x != 0 && velocity.y != 0) {
        velocity.x *= 0.7071f;
        velocity.y *= 0.7071f;
    }

    player.isMoving = (velocity.x != 0 || velocity.y != 0);

    // Collision X
    float nextX = player.pos.x + velocity.x * dt;
    // !isSolid(nextX, player.pos.y)
    if (1)
        player.pos.x = nextX;

    // Collision Y
    float nextY = player.pos.y + velocity.y * dt;
    // if (!isSolid(player.pos.x, nextY))
    if (1)
        player.pos.y = nextY;
}

void drawPlayer(sf::RenderWindow& window) {
    playerRect.setPosition(player.pos);
    window.draw(playerRect);
}