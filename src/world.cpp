#include "world.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace sf;

static int findMapIndex(  World& world,   string& mapName) {
    for (int i = 0; i < world.mapCount; i++) {
        if (world.mapNames[i] == mapName) return i;
    }

    return -1;
}

static int findLayoutIndex(  World& world,   string& mapName) {
    for (int i = 0; i < world.layoutCount; i++) {
        if (world.mapLayouts[i].name == mapName) return i;
    }

    return -1;
}

static int findSpawnListIndex(  World& world,   string& mapName) {
    for (int i = 0; i < world.spawnListCount; i++)
        if (world.entitySpawns[i].mapName == mapName) return i;
    return -1;
}

static int findDirtyIndex(  World& world,   string& mapName) {
    for (int i = 0; i < world.dirtyFlagCount; i++)
        if (world.mapDirtyFlags[i].mapName == mapName) return i;
    return -1;
}

bool worldLoadAllMaps(World& world) {
    //input file stream to start making a configFile able to be read 
    ifstream configFile("assets/world_config.json");
    if (!configFile.is_open()) {
        cout << "[ERROR] Cannot open assets/world_config.json" << endl;
        return false;
    }

    world.mapCount = 0;
    world.layoutCount = 0;
    world.spawnListCount = 0;
    world.dirtyFlagCount = 0;

    string line, mapName;
    int worldX = 0, worldY = 0, pixelWidth = 0, pixelHeight = 0, mapCount = 0;

    while (getline(configFile, line)) { // line line
        if (line.find("\"name\"") != string::npos) { // npos = no position
            int start = line.find( "\"",  line.find(":") + 1 ) + 1; //goal,strt idx
            int end = line.find("\"", start);
            mapName = line.substr(start, end - start); // strtidx,length
        }
        //stoi = string to integer
        if (line.find("\"worldX\"") != string::npos)
            worldX = stoi(line.substr(line.find(':') + 1));
        if (line.find("\"worldY\"") != string::npos)
            worldY = stoi(line.substr(line.find(':') + 1));
        if (line.find("\"pixelWidth\"") != string::npos)
            pixelWidth = stoi(line.substr(line.find(':') + 1));
        if (line.find("\"pixelHeight\"") != string::npos) {
            pixelHeight = stoi(line.substr(line.find(':') + 1));

            if (!mapName.empty() && world.mapCount < MAX_MAPS) {
                string jsonPath = "assets/maps/" + mapName + "/" + mapName + ".json";
                GameMap newMap;
                if (!loadMapFromJSON(newMap, jsonPath)) {
                    cout << "[ERROR] Failed to load map: " <<mapName << endl;
                    return false;
                }

                int idx = world.mapCount;
                world.maps[idx] = newMap;
                world.mapNames[idx] = mapName;
                world.mapCount++;

                if (world.layoutCount < MAX_MAPS) {
                    MapLayout& layout = world.mapLayouts[world.layoutCount++]; //refrence var to the original item not a copy not a * to avoid-> and indep var just a esm dal3
                    layout.name = mapName;
                    layout.worldOffsetX = worldX;
                    layout.worldOffsetY = worldY;
                    layout.pixelWidth = pixelWidth;
                    layout.pixelHeight = pixelHeight;
                }

                if (world.dirtyFlagCount < MAX_MAPS) {
                    MapDirtyEntry& entry = world.mapDirtyFlags[world.dirtyFlagCount++];
                    entry.mapName = mapName;
                    entry.dirty = false;
                }

                cout << "[LOAD] Map: " << mapName << " at (" << worldX << ", " << worldY << ")" << endl;
                mapCount++;
                mapName = "";
            }
        }
    }

    if (world.layoutCount > 0) {
        world.currentMapName = "outside";
        cout << "[SUCCESS] World loaded " << mapCount << " maps. Starting in: outside" << endl;
        return true;
    }
    cout << "[ERROR] No maps loaded" << endl;
    return false;
}

GameMap* worldGetCurrentMap(World& world) {
    int idx = findMapIndex(world, world.currentMapName);
    if (idx >= 0) return &world.maps[idx];
    cout << "[ERROR] Current map '" << world.currentMapName << "' not found" << endl;
    return nullptr;
}

void worldSetCurrentMap(World& world,   string& mapName) {
    string oldMapName = world.currentMapName;
    int idx = findMapIndex(world, mapName);
    if (idx < 0) {
        cout << "[ERROR] Map '" << mapName<< "' does not exist" << endl; return;
    }
    world.currentMapName = mapName;
    if (!oldMapName.empty()) {
        worldOnPlayerLeaveMap(world, oldMapName);
    }
    worldOnPlayerEnterMap(world, mapName);
    cout << "[MAP CHANGE] " << oldMapName << " : " <<mapName << endl;
}

GameMap* worldGetMapByName(World& world,   string& mapName) {
    int idx = findMapIndex(world, mapName);
    if (idx >= 0) return &world.maps[idx];
    cout << "[ERROR] Map '" << mapName << "' not found" << endl;
    return nullptr;
}

Vector2f worldMapToWorldCoords(  World& world,   string& mapName, Vector2f localPos) {
    int idx = findLayoutIndex(world, mapName);
    if (idx >= 0)
        return Vector2f(localPos.x + world.mapLayouts[idx].worldOffsetX,
                        localPos.y + world.mapLayouts[idx].worldOffsetY);
    cout << "[ERROR] Map '" << mapName << "' not found in layouts" << endl;
    return Vector2f(0, 0);
}// WorldPos = LocalPos + Offset

Vector2f worldWorldToMapCoords(  World& world,   string& mapName, Vector2f worldPos) {
    int idx = findLayoutIndex(world, mapName);
    if (idx >= 0)
        return Vector2f(worldPos.x - world.mapLayouts[idx].worldOffsetX,
                        worldPos.y - world.mapLayouts[idx].worldOffsetY);
    cout << "[ERROR] Map '" << mapName << "' not found in layouts" << endl;
    return Vector2f(0, 0);
}//LocalPos = WorldPos - Offset

string worldGetMapAtWorldPosition(  World& world, float worldX, float worldY) {
    for (int i = 0; i < world.layoutCount; i++) {
          MapLayout& l = world.mapLayouts[i];
        if (worldX >= l.worldOffsetX && worldX < l.worldOffsetX + l.pixelWidth && worldY >= l.worldOffsetY && worldY < l.worldOffsetY + l.pixelHeight)
            return l.name;
    }
    return "";
}// el x , y mahbosa gowa ana rectangle fe el le3ba w mnha ba7dd





void worldSaveMapState(World& world,   string& mapName) {
    cout << "[SAVE] Map '" << mapName << "' state saved (placeholder)" << endl;
}

void worldLoadMapState(World& world,   string& mapName) {
    cout << "[LOAD] Map '" << mapName << "' state loaded (placeholder)" << endl;
}

void worldOnPlayerEnterMap(World& world,   string& mapName) {
    cout << "[ENTER] Player entered map: " << mapName << endl;
}

void worldOnPlayerLeaveMap(World& world,   string& mapName) {
    cout << "[LEAVE] Player left map: " << mapName << endl;
}

void worldChangeMapTileSet(World& world,   string& mapName,   string& cursedTexturePath) {
    int idx = findMapIndex(world, mapName);
    if (idx >= 0) {
        mapSwapTileset(world.maps[idx], cursedTexturePath);
        cout << "[CURSE] Map " << mapName << " is now CURSED!" << endl;
    }
}