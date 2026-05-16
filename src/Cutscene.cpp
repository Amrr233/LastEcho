#include "Cutscene.h"
#include "DialogueManager.h"
#include "NPC.h"
#include <iostream>
#include <cmath>
#include "player.h"

CutsceneRuntime g_cutscene;
static Cutscene g_activeScene;
extern Player player;
extern World  world;

void startGenericCutscene(std::string id, std::vector<CutsceneAction> steps) {
    g_activeScene.cutsceneID       = id;
    g_activeScene.actions          = steps;
    g_activeScene.currentActionIdx = 0;
    g_activeScene.isActive         = true;

    // Reset action timer for the first step
    if (!g_activeScene.actions.empty())
        g_activeScene.actions[0].actionTimer = 0.f;

    g_cutscene.currentCutscene = &g_activeScene;
    g_cutscene.isActive        = true;
    g_cutscene.characterCount  = 0;

    for (int i = 0; i < 5; i++)
        g_cutscene.characters[i].currentEmotion = EMOTION_NONE;

    std::cout << "[CUTSCENE] Started: " << id << std::endl;
}

void stopCutscene() {
    g_cutscene.isActive = false;
    if (g_cutscene.currentCutscene)
        g_cutscene.currentCutscene->isActive = false;
    g_cutscene.currentCutscene = nullptr;
}

static int getOrCreateChar(const std::string& name) {
    for (int i = 0; i < g_cutscene.characterCount; i++)
        if (g_cutscene.characters[i].name == name) return i;

    if (g_cutscene.characterCount < 5) {
        int idx = g_cutscene.characterCount++;
        g_cutscene.characters[idx] = {};
        g_cutscene.characters[idx].name = name;
        return idx;
    }
    return -1;
}

void updateCutscene(float deltaTime) {
    if (!g_cutscene.isActive || !g_cutscene.currentCutscene) return;

    Cutscene& scene = *g_cutscene.currentCutscene;
    if (scene.currentActionIdx >= (int)scene.actions.size()) {
        stopCutscene();
        return;
    }

    CutsceneAction& action = scene.actions[scene.currentActionIdx];
    action.actionTimer += deltaTime;

    for (int i = 0; i < g_cutscene.characterCount; i++) {
        auto& cs = g_cutscene.characters[i];

        if (cs.currentEmotion != EMOTION_NONE) {
            cs.emotionTimer += deltaTime;
            if (cs.emotionTimer >= 0.15f) {
                cs.emotionTimer = 0.f;
                cs.currentFrame = (cs.currentFrame + 1) % 4;
            }
        }


        if (!cs.isStarted && cs.isMoving) {
            cs.delayTimer += deltaTime;
            if (cs.delayTimer >= cs.startDelay) {
                cs.isStarted = true;
            }
        }


        if (cs.isStarted && cs.isMoving) {
            sf::Vector2f diff = cs.targetPos - cs.pos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (dist < 15.0f) {
                cs.pos       = cs.targetPos;
                cs.isMoving  = false;
                cs.isStarted = false;
                cs.isArrived = true;
                setNPCFrame(cs.name, 0); // Reset to idle frame
            } else {
                sf::Vector2f dir = diff / dist;
                cs.pos += dir * cs.speed * deltaTime;


                int direction;
                if (std::abs(dir.x) > std::abs(dir.y))
                    direction = (dir.x > 0) ? 3 : 2; // East or West
                else
                    direction = (dir.y > 0) ? 0 : 1; // South or North

                updateNPCAnimation(cs.name, direction, deltaTime);
            }
            updateNPCPosition(cs.name, cs.pos);
        }
    }

    bool stepFinished = false;


    switch (action.type) {

    case CUTSCENE_WAIT:
        if (action.actionTimer >= action.waitTime) stepFinished = true;
        break;

    case CUTSCENE_SET_DIRECTION:
        updateNPCAnimation(action.characterName, action.direction, 0.0f);
        setNPCFrame(action.characterName, 0);
        stepFinished = true;
        break;

    case CUTSCENE_EMOTION: {
        if (action.actionTimer <= deltaTime + 0.001f) {
            int idx = getOrCreateChar(action.characterName);
            if (idx != -1) {
                g_cutscene.characters[idx].currentEmotion = action.emotion;
                g_cutscene.characters[idx].currentFrame   = 0;
                g_cutscene.characters[idx].emotionTimer   = 0.f;
            }
        }
        if (action.actionTimer >= action.emotionDuration) {
            for (int i = 0; i < g_cutscene.characterCount; i++)
                if (g_cutscene.characters[i].name == action.characterName)
                    g_cutscene.characters[i].currentEmotion = EMOTION_NONE;
            stepFinished = true;
        }
        break;
    }

    case CUTSCENE_MOVE: {
        if (action.actionTimer <= deltaTime + 0.001f) {
            int idx = getOrCreateChar(action.characterName);
            if (idx != -1) {
                g_cutscene.characters[idx].pos        = getNPCPosition(action.characterName);
                g_cutscene.characters[idx].targetPos  = { action.targetX, action.targetY };
                g_cutscene.characters[idx].isMoving   = true;
                g_cutscene.characters[idx].isStarted  = true;
                g_cutscene.characters[idx].speed      = 160.f;
                g_cutscene.characters[idx].startDelay = 0.f;
                g_cutscene.characters[idx].delayTimer = 0.f;
            }
        }
        int idx = -1;
        for (int i = 0; i < g_cutscene.characterCount; i++)
            if (g_cutscene.characters[i].name == action.characterName) { idx = i; break; }

        if (idx != -1) {
            sf::Vector2f diff = g_cutscene.characters[idx].targetPos - g_cutscene.characters[idx].pos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (!g_cutscene.characters[idx].isMoving || dist < action.completionDistance)
                stepFinished = true;
        }
        break;
    }

    case CUTSCENE_SPEAK:
        if (action.actionTimer <= deltaTime + 0.001f)
            startDialogue(action.characterName, action.lines, action.lineCount,
                          getNPCAvatar(action.characterName));
        if (!isDialogueActive()) stepFinished = true;
        break;

    case CUTSCENE_CHANGE_MAP:
        for (int i = 0; i < npcCount; i++) {
            if (allNPCs[i].name == action.characterName) {
                allNPCs[i].currentMap = action.targetMap;
                allNPCs[i].pos        = action.spawnPos;
                allNPCs[i].sprite.setPosition(action.spawnPos);
                break;
            }
        }
        stepFinished = true;
        break;

    case CUTSCENE_PLAYER_MOVE: {
        sf::Vector2f target(action.targetX, action.targetY);
        sf::Vector2f diff = target - player.pos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < 20.0f) {
            player.pos = target;
            updatePlayerAnimation(player.facing, deltaTime, false);
            stepFinished = true;
        } else {
            sf::Vector2f dir = diff / dist;
            player.pos += dir * 180.f * deltaTime;
            Direction moveDir = player.facing;
            if (std::abs(dir.x) > std::abs(dir.y)) moveDir = (dir.x > 0) ? EAST : WEST;
            else moveDir = (dir.y > 0) ? SOUTH : NORTH;
            updatePlayerAnimation(moveDir, deltaTime, true);
        }
        break;
    }

    case CUTSCENE_PLAYER_TELEPORT: {
        extern float fadeAlpha;
        extern bool  isFading;
        worldSetCurrentMap(world, action.targetMap);
        player.pos = action.spawnPos;
        player.sprite.setPosition(player.pos);
        fadeAlpha = 255.0f;
        isFading  = true;
        stepFinished = true;
        break;
    }

    case CUTSCENE_MOVE_GROUP: {
        if (!action.isStarted) {
            action.isStarted = true;
            g_cutscene.playerMoving     = false;
            g_cutscene.playerStarted    = false;
            g_cutscene.playerDelayTimer = 0.f;

            // Initialize all movers in the group
            for (auto& mover : action.moveGroup.movers) {
                if (mover.type == MoverTarget::NPC) {
                    int idx = getOrCreateChar(mover.npcName);
                    if (idx != -1) {
                        g_cutscene.characters[idx].pos        = getNPCPosition(mover.npcName);
                        g_cutscene.characters[idx].targetPos  = { mover.targetX, mover.targetY };
                        g_cutscene.characters[idx].isMoving   = true;
                        g_cutscene.characters[idx].isStarted  = (mover.startDelay <= 0.f);
                        g_cutscene.characters[idx].isArrived  = false;
                        g_cutscene.characters[idx].speed      = mover.speed;
                        g_cutscene.characters[idx].startDelay = mover.startDelay;
                        g_cutscene.characters[idx].delayTimer = 0.f;
                    }
                } else {
                    g_cutscene.playerMoving     = true;
                    g_cutscene.playerStarted    = (mover.startDelay <= 0.f);
                    g_cutscene.playerStartDelay = mover.startDelay;
                    g_cutscene.playerDelayTimer = 0.f;
                    g_cutscene.playerTargetX    = mover.targetX;
                    g_cutscene.playerTargetY    = mover.targetY;
                    g_cutscene.playerSpeed      = mover.speed;
                }
            }
        }

        bool allDone = true;

        if (g_cutscene.playerMoving) {
            if (!g_cutscene.playerStarted) {
                g_cutscene.playerDelayTimer += deltaTime;
                if (g_cutscene.playerDelayTimer >= g_cutscene.playerStartDelay)
                    g_cutscene.playerStarted = true;
                allDone = false;
            } else {
                sf::Vector2f target(g_cutscene.playerTargetX, g_cutscene.playerTargetY);
                sf::Vector2f diff = target - player.pos;
                float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                if (dist > action.completionDistance) {
                    allDone = false;
                    sf::Vector2f dir = diff / dist;
                    player.pos += dir * g_cutscene.playerSpeed * deltaTime;
                    Direction moveDir = player.facing;
                    if (std::abs(dir.x) > std::abs(dir.y)) moveDir = (dir.x > 0) ? EAST : WEST;
                    else moveDir = (dir.y > 0) ? SOUTH : NORTH;
                    updatePlayerAnimation(moveDir, deltaTime, true);
                } else {
                    player.pos = target;
                    updatePlayerAnimation(player.facing, deltaTime, false);
                    g_cutscene.playerMoving = false;
                }
            }
        }

        for (auto& mover : action.moveGroup.movers) {
            if (mover.type == MoverTarget::NPC) {
                int idx = -1;
                for (int i = 0; i < g_cutscene.characterCount; i++)
                    if (g_cutscene.characters[i].name == mover.npcName) { idx = i; break; }
                if (idx != -1 && !g_cutscene.characters[idx].isArrived) allDone = false;
            }
        }
        if (allDone) stepFinished = true;
        break;
    }
    }

    if (stepFinished) {
        scene.currentActionIdx++;
        if (scene.currentActionIdx < (int)scene.actions.size())
            scene.actions[scene.currentActionIdx].actionTimer = 0.f;
    }
}

sf::IntRect calculateEmoteRect(Emotion emotion, int frame) {
    if (emotion == EMOTION_NONE) return sf::IntRect(0, 0, 0, 0);
    // Emotes are 16x16 on the sheet, assuming emotion maps to row
    return sf::IntRect(frame * 16, static_cast<int>(emotion) * 16, 16, 16);
}

void initCutsceneSystem() {
    if (!g_cutscene.emoteSheet.loadFromFile("assets/sprites/emotes/emotes.png"))
        std::cout << "[ERROR] Could not load emotes.png" << std::endl;

    g_cutscene.emoteSprite.setTexture(g_cutscene.emoteSheet);
    g_cutscene.emoteSprite.setOrigin(8.f, 16.f); // Bottom center
    g_cutscene.emoteSprite.setScale(3.0f, 3.0f);
}

void drawCutsceneOverlay(sf::RenderWindow& window, sf::Font& font) {
    if (!g_cutscene.isActive) return;

    for (int i = 0; i < g_cutscene.characterCount; i++) {
        auto& cs = g_cutscene.characters[i];
        if (cs.currentEmotion != EMOTION_NONE) {
            sf::Vector2f pos = getNPCPosition(cs.name);
            g_cutscene.emoteSprite.setTextureRect(
                calculateEmoteRect(cs.currentEmotion, cs.currentFrame));
            g_cutscene.emoteSprite.setPosition(pos.x + 5.f, pos.y -48.f);

            window.draw(g_cutscene.emoteSprite);
        }
    }
}

bool isCutsceneActive() { return g_cutscene.isActive; }