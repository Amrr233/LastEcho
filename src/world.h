#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "GameMap.h"

#include "phase.h"

using namespace sf;

using namespace std;

const int MAX_MAPS           = 16;
const int MAX_SPAWNS_PER_MAP = 10;

struct MapLayout {
    string name;
    int worldOffsetX;
    int worldOffsetY;
    int pixelWidth;
    int pixelHeight;};
// every map has a layout
//offsest: how far is the map from the left,top of the whole world map

struct EntitySpawn {
    string   entityId;
    Vector2f localPos;
};
//to spawn anything with a specific id in specific map (local)

struct MapSpawnList {
    string      mapName;
    EntitySpawn spawns[MAX_SPAWNS_PER_MAP];
    int         count;
};
//list for the things that spawned in somemap and a counter to count things that its really there

struct MapDirtyEntry {
    string mapName;
    bool   dirty;
};
// if player changed anything or anything spawned we make it true so wwe can save it later

struct World {
    GameMap       maps[MAX_MAPS];
    string        mapNames[MAX_MAPS];
    int           mapCount;
    MapLayout     mapLayouts[MAX_MAPS];
    int           layoutCount;
    MapSpawnList  entitySpawns[MAX_MAPS];
    int           spawnListCount;
    MapDirtyEntry mapDirtyFlags[MAX_MAPS];
    int           dirtyFlagCount;
    string        currentMapName;
    PhaseSystem   phaseSys;

    HintSystem    hintSys;
};

bool      worldLoadAllMaps(World& world); //pass by refrence to get the original world not a copy
GameMap*  worldGetCurrentMap(World& world);
void      worldSetCurrentMap(World& world,string& mapName);
GameMap*  worldGetMapByName(World& world,string& mapName); // from mapNames
Vector2f  worldMapToWorldCoords( World& world, string& mapName,Vector2f localPos);
Vector2f  worldWorldToMapCoords( World& world, string& mapName,Vector2f worldPos);
string    worldGetMapAtWorldPosition( World& world, float worldX,float worldY);
void      worldSaveMapState(World& world,  string& mapName);
void      worldLoadMapState(World& world,  string& mapName);
void      worldOnPlayerEnterMap(World& world,  string& mapName);
void      worldOnPlayerLeaveMap(World& world,  string& mapName);
void      worldChangeMapTileSet(World& world,  string& mapName,  string& cursedTexturePath);