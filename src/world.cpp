#include "world.h"
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;
using namespace sf;

// ============================================
// SIMPLIFIED VERSION WITHOUT nlohmann/json
// ============================================
// This version manually parses world_config.json
// No external library needed!

bool worldLoadAllMaps(World& world) {
    ifstream configFile("assets/world_config.json");
    if (!configFile.is_open()) {
        cerr << "[ERROR] Cannot open assets/world_config.json" << endl;
        return false;
    }

    world.maps.clear();
    world.mapLayouts.clear();
    world.entitySpawns.clear();
    world.mapDirtyFlags.clear();

    string line;
    string mapName;
    int worldX = 0, worldY = 0, pixelWidth = 0, pixelHeight = 0;
    int mapCount = 0;

    while (getline(configFile, line)) {
        // Parse JSON manually (very basic, but works)

        // Look for "name" field
        if (line.find("\"name\"") != string::npos) {
            size_t start = line.find("\"", line.find(":") + 1) + 1;
            size_t end = line.find("\"", start);
            mapName = line.substr(start, end - start);
        }

        // Look for "worldX" field
        if (line.find("\"worldX\"") != string::npos) {
            size_t start = line.find(':') + 1;
            worldX = stoi(line.substr(start));
        }

        // Look for "worldY" field
        if (line.find("\"worldY\"") != string::npos) {
            size_t start = line.find(":") + 1;
            worldY = stoi(line.substr(start));
        }

        // Look for "pixelWidth" field
        if (line.find("\"pixelWidth\"") != string::npos) {
            size_t start = line.find(":") + 1;
            pixelWidth = stoi(line.substr(start));
        }

        // Look for "pixelHeight" field
        if (line.find("\"pixelHeight\"") != string::npos) {
            size_t start = line.find(":") + 1;
            pixelHeight = stoi(line.substr(start));

            // We have all fields, load the map
            if (!mapName.empty()) {
                string jsonPath = "assets/maps/" + mapName + "/" + mapName + ".json";
                GameMap newMap;

                if (!loadMapFromJSON(newMap, jsonPath)) {
                    cerr << "[ERROR] Failed to load map: " << mapName << endl;
                    return false;
                }

                world.maps[mapName] = newMap;

                MapLayout layout;
                layout.name = mapName;
                layout.worldOffsetX = worldX;
                layout.worldOffsetY = worldY;
                layout.pixelWidth = pixelWidth;
                layout.pixelHeight = pixelHeight;

                world.mapLayouts.push_back(layout);
                world.mapDirtyFlags[mapName] = false;

                cout << "[LOADED] Map: " << mapName
                     << " at world position (" << worldX << ", " << worldY << ")" << endl;

                mapCount++;
                mapName = "";
            }
        }
    }

    if (!world.mapLayouts.empty()) {
        world.currentMapName =  "outside";
        cout << "[SUCCESS] World loaded " << mapCount << " maps. Starting in: "
             << world.currentMapName << endl;
        return true;
    } else {
        cerr << "[ERROR] No maps loaded" << endl;
        return false;
    }
}

// ============================================
// REST OF FUNCTIONS (SAME AS BEFORE)
// ============================================

GameMap* worldGetCurrentMap(World& world) {
    auto it = world.maps.find(world.currentMapName);
    if (it != world.maps.end()) {
        return &(it->second);
    } else {
        cerr << "[ERROR] Current map '" << world.currentMapName << "' not found" << endl;
        return nullptr;
    }
}

void worldSetCurrentMap(World& world, const string& mapName) {
    string oldMapName = world.currentMapName;

    if (world.maps.find(mapName) == world.maps.end()) {
        cerr << "[ERROR] Map '" << mapName << "' does not exist" << endl;
        return;
    }

    world.currentMapName = mapName;

    if (!oldMapName.empty()) {
        worldOnPlayerLeaveMap(world, oldMapName);
    }

    worldOnPlayerEnterMap(world, mapName);

    cout << "[MAP CHANGE] " << oldMapName << " → " << mapName << endl;
}

GameMap* worldGetMapByName(World& world, const string& mapName) {
    auto it = world.maps.find(mapName);
    if (it != world.maps.end()) {
        return &(it->second);
    } else {
        cerr << "[ERROR] Map '" << mapName << "' not found" << endl;
        return nullptr;
    }
}

vector<string> worldGetAllMapNames(const World& world) {
    vector<string> names;
    for (const auto& pair : world.maps) {
        names.push_back(pair.first);
    }
    return names;
}

Vector2f worldMapToWorldCoords(const World& world, const string& mapName, Vector2f localPos) {
    for (const auto& layout : world.mapLayouts) {
        if (layout.name == mapName) {
            Vector2f worldPos;
            worldPos.x = localPos.x + layout.worldOffsetX;
            worldPos.y = localPos.y + layout.worldOffsetY;
            return worldPos;
        }
    }
    cerr << "[ERROR] Map '" << mapName << "' not found in layouts" << endl;
    return Vector2f(0, 0);
}

Vector2f worldWorldToMapCoords(const World& world, const string& mapName, Vector2f worldPos) {
    for (const auto& layout : world.mapLayouts) {
        if (layout.name == mapName) {
            Vector2f localPos;
            localPos.x = worldPos.x - layout.worldOffsetX;
            localPos.y = worldPos.y - layout.worldOffsetY;
            return localPos;
        }
    }
    cerr << "[ERROR] Map '" << mapName << "' not found in layouts" << endl;
    return Vector2f(0, 0);
}

string worldGetMapAtWorldPosition(const World& world, float worldX, float worldY) {
    for (const auto& layout : world.mapLayouts) {
        bool insideX = (worldX >= layout.worldOffsetX) &&
                       (worldX < layout.worldOffsetX + layout.pixelWidth);
        bool insideY = (worldY >= layout.worldOffsetY) &&
                       (worldY < layout.worldOffsetY + layout.pixelHeight);

        if (insideX && insideY) {
            return layout.name;
        }
    }
    return "";
}

FloatRect worldGetMapWorldBounds(const World& world, const string& mapName) {
    for (const auto& layout : world.mapLayouts) {
        if (layout.name == mapName) {
            return FloatRect(layout.worldOffsetX, layout.worldOffsetY,
                           layout.pixelWidth, layout.pixelHeight);
        }
    }
    cerr << "[ERROR] Map '" << mapName << "' not found" << endl;
    return FloatRect(0, 0, 0, 0);
}

void worldRegisterEntitySpawnPoint(World& world, const string& mapName,
                                   const string& entityId, Vector2f localPos) {
    EntitySpawn spawn;
    spawn.entityId = entityId;
    spawn.localPos = localPos;
    world.entitySpawns[mapName].push_back(spawn);
    cout << "[REGISTERED] Entity '" << entityId << "' in map '" << mapName << endl;
}

vector<EntitySpawn> worldGetEntitySpawnPoints(const World& world, const string& mapName) {
    auto it = world.entitySpawns.find(mapName);
    if (it != world.entitySpawns.end()) {
        return it->second;
    } else {
        return vector<EntitySpawn>();
    }
}

void worldSaveMapState(World& world, const string& mapName) {
    cout << "[SAVE] Map '" << mapName << "' state saved (placeholder)" << endl;
}

void worldLoadMapState(World& world, const string& mapName) {
    cout << "[LOAD] Map '" << mapName << "' state loaded (placeholder)" << endl;
}

void worldMarkMapDirty(World& world, const string& mapName) {
    world.mapDirtyFlags[mapName] = true;
    cout << "[DIRTY] Map '" << mapName << "' marked as changed" << endl;
}

bool worldIsMapDirty(const World& world, const string& mapName) {
    auto it = world.mapDirtyFlags.find(mapName);
    if (it != world.mapDirtyFlags.end()) {
        return it->second;
    } else {
        return false;
    }
}

void worldOnPlayerEnterMap(World& world, const string& mapName) {
    cout << "[ENTER] Player entered map: " << mapName << endl;
}

void worldOnPlayerLeaveMap(World& world, const string& mapName) {
    cout << "[LEAVE] Player left map: " << mapName << endl;
}