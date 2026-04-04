#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

// ==============================
// CONSTANTS
// ==============================
const int TILE_SIZE  = 32;
const int SCREEN_W   = 1280;
const int SCREEN_H   = 720;
const int PHASE_COUNT = 7;
const int MAX_NPCS   = 20;
const int MAX_ITEMS  = 16;
const int MAX_PORTALS = 8;

// ==============================
// ENUMS
// ==============================
enum AppState {
    STATE_MENU,        // ← أضف ده
    STATE_PLAYING,
    STATE_DIALOGUE,
    STATE_BOSS_FIGHT,
    STATE_PAUSED,
    STATE_GAME_OVER,
    STATE_SETTINGS,    // ← أضف ده
    STATE_LOAD         // ← أضف ده
};

enum Direction {
    DIR_IDLE,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

// ==============================
// STRUCTS
// ==============================
struct Player {
    sf::Vector2f pos;
    sf::Vector2f velocity;
    Direction    facing;
    float        speed;
    int          hp;
    int          maxHp;
    int          xp;
    int          level;
    int          currentPhase;
    int          frameIndex;
    float        aniTimer;
    bool         isMoving;
};

struct TileMap {
    std::vector<int> groundLayer;
    std::vector<int> collisionLayer;
    std::vector<int> detailLayer;
    int widthTiles;
    int heightTiles;
    int tileSize;
};

struct GameState {
    AppState currentState;
    float    deltaTime;
    int      currentPhase;
    bool     phaseCompleted[PHASE_COUNT];
    bool     guidesMet[PHASE_COUNT];
    bool     bossDefeated[PHASE_COUNT];
};

// ==============================
// GLOBALS — معرّفين هنا بس
// ==============================
extern sf::RenderWindow window;
extern GameState        gState;
extern Player           player;
extern TileMap          currentMap;