#pragma once
#include <SFML/Graphics.hpp>

const int TILE_SIZE   = 32;
const int SCREEN_W    = 1280;
const int SCREEN_H    = 720;
const int PHASE_COUNT = 7;
const int MAX_NPCS    = 20;
const int MAX_ITEMS   = 16;
const int MAX_PORTALS = 8;
const int MAX_WAYPOINTS = 100;
const int MAX_DIALOGUE_LINES = 100;

// enum for the states
enum AppState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_SETTINGS,
    STATE_LOAD ,
};


struct GameState {
    AppState currentState;
    float    deltaTime;
    int      currentPhase;
};


enum BossPhase {
    BOSS_PHASE_1,   // 100% to 66% HP
    BOSS_PHASE_2,   // 66% to 33% HP
    BOSS_PHASE_3    // 33% to 0% HP
};

enum BossState {
    BOSS_IDLE,
    BOSS_WALKING,
    BOSS_ATTACKING,
    BOSS_HURT,
    BOSS_DEAD
};


extern sf::RenderWindow window;
extern GameState        gState;
extern AppState         last_state;
