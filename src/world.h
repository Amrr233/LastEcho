#ifndef WORLD_H
#define WORLD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>
#include "GameMap.h"

using namespace sf;
using namespace std;

// ============================================
// STRUCT 1: MapLayout
// ============================================
// PURPOSE: Describes where one map sits in world space
// EXAMPLE:
// If "clinic" map is 1080 pixels wide × 954 pixels tall
// And positioned at world (960, 0):
//   MapLayout {
//     name = "clinic"
//     worldOffsetX = 960  ← left edge position
//     worldOffsetY = 0    ← top edge position
//     pixelWidth = 1080   ← width in pixels
//     pixelHeight = 954   ← height in pixels
//   }

struct MapLayout {
    string name;
    int worldOffsetX;
    int worldOffsetY;
    int pixelWidth;
    int pixelHeight;
};

// ============================================
// STRUCT 2: EntitySpawn
// ============================================
// PURPOSE: Where NPCs, items, enemies spawn
// EXAMPLE:
// entityId = "npc_doctor"
// localPos = {100, 200}  ← position inside map

struct EntitySpawn {
    string entityId;
    Vector2f localPos;
};

// ============================================
// STRUCT 3: World (THE MAIN STRUCT)
// ============================================
// Holds ALL game world data

struct World {
    // All maps in memory (name → GameMap)
    map<string, GameMap> maps;

    // Map positions in world space
    vector<MapLayout> mapLayouts;

    // Entity spawn points per map
    map<string, vector<EntitySpawn>> entitySpawns;

    // Track which maps changed (for saving)
    map<string, bool> mapDirtyFlags;

    // Which map player is in NOW
    string currentMapName;
};

// ============================================
// FUNCTION DECLARATIONS
// ============================================

// Load all maps from disk at startup
bool worldLoadAllMaps(World& world);

// Get pointer to current map
GameMap* worldGetCurrentMap(World& world);

// Switch to different map
void worldSetCurrentMap(World& world, const string& mapName);

// Get any map by name
GameMap* worldGetMapByName(World& world, const string& mapName);

// Get list of all map names
vector<string> worldGetAllMapNames(const World& world);

// Convert local map coords to world coords
Vector2f worldMapToWorldCoords(const World& world, const string& mapName, Vector2f localPos);

// Convert world coords to local map coords
Vector2f worldWorldToMapCoords(const World& world, const string& mapName, Vector2f worldPos);

// Check which map contains a world position
string worldGetMapAtWorldPosition(const World& world, float worldX, float worldY);

// Get map's world-space boundaries
FloatRect worldGetMapWorldBounds(const World& world, const string& mapName);

// Register entity spawn point
void worldRegisterEntitySpawnPoint(World& world, const string& mapName,
                                   const string& entityId, Vector2f localPos);

// Get all spawn points in a map
vector<EntitySpawn> worldGetEntitySpawnPoints(const World& world, const string& mapName);

// Save map state to disk
void worldSaveMapState(World& world, const string& mapName);

// Load map state from disk
void worldLoadMapState(World& world, const string& mapName);

// Mark map as changed
void worldMarkMapDirty(World& world, const string& mapName);

// Check if map was changed
bool worldIsMapDirty(const World& world, const string& mapName);

// HOOKS for your team to implement:
void worldOnPlayerEnterMap(World& world, const string& mapName);
void worldOnPlayerLeaveMap(World& world, const string& mapName);

#endif