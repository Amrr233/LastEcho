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
    g_activeScene.cutsceneID = id;
    g_activeScene.actions = steps;
    g_activeScene.currentActionIdx = 0;
    g_activeScene.isActive = true;

    if(!g_activeScene.actions.empty()) g_activeScene.actions[0].actionTimer = 0.f;

    g_cutscene.currentCutscene = &g_activeScene;
    g_cutscene.isActive = true;
    g_cutscene.characterCount = 0;

    // تصفير الايموتات القديمة لأي شخصية
    for(int i=0; i<5; i++) g_cutscene.characters[i].currentEmotion = EMOTION_NONE;

    std::cout << "[CUTSCENE] Started: " << id << std::endl;
}

void stopCutscene() {
    g_cutscene.isActive = false;
    if (g_cutscene.currentCutscene) g_cutscene.currentCutscene->isActive = false;
    g_cutscene.currentCutscene = nullptr;
}

void updateCutscene(float deltaTime) {
    if (!g_cutscene.isActive || !g_cutscene.currentCutscene) return;

    Cutscene& scene = *g_cutscene.currentCutscene;
    if (scene.currentActionIdx >= scene.actions.size()) {
        stopCutscene();
        return;
    }

    CutsceneAction& action = scene.actions[scene.currentActionIdx];
    action.actionTimer += deltaTime;

    // 1. تحريك الـ NPCs والـ Animation الخاص بيهم
    for (int i = 0; i < g_cutscene.characterCount; i++) {
        auto& cs = g_cutscene.characters[i];

        // تحريك الـ Sprite بتاع الايموت (4 فريمات)
        if (cs.currentEmotion != EMOTION_NONE) {
            cs.emotionTimer += deltaTime;
            if (cs.emotionTimer >= 0.15f) { // سرعة فريمات الايموت
                cs.emotionTimer = 0.f;
                cs.currentFrame = (cs.currentFrame + 1) % 4;
            }
        }

        // تحريك الـ NPC نفسه
        if (cs.isMoving) {
            sf::Vector2f diff = cs.targetPos - cs.pos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            if (dist < 15.0f) { // زيادة المسافة لنعومة الانتقال
                cs.pos = cs.targetPos;
                cs.isMoving = false;
                setNPCFrame(cs.name, 0);
            } else {
                sf::Vector2f dir = diff / dist;
                cs.pos += dir * 160.f * deltaTime; // سرعة عمرو

                int direction;
                if (std::abs(dir.x) > std::abs(dir.y)) direction = (dir.x > 0) ? 3 : 2; // East/West
                else direction = (dir.y > 0) ? 0 : 1; // South/North

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

    case CUTSCENE_EMOTION:
        if (action.actionTimer <= deltaTime + 0.001f) {
            int charIdx = -1;
            for(int i=0; i<g_cutscene.characterCount; i++)
                if(g_cutscene.characters[i].name == action.characterName) charIdx = i;

            if(charIdx == -1 && g_cutscene.characterCount < 5) {
                charIdx = g_cutscene.characterCount++;
                g_cutscene.characters[charIdx].name = action.characterName;
            }
            if(charIdx != -1) {
                g_cutscene.characters[charIdx].currentEmotion = action.emotion;
                g_cutscene.characters[charIdx].currentFrame = 0;
                g_cutscene.characters[charIdx].emotionTimer = 0.f;
            }
        }
        if (action.actionTimer >= action.emotionDuration) {
            for(int i=0; i<g_cutscene.characterCount; i++)
                if(g_cutscene.characters[i].name == action.characterName)
                    g_cutscene.characters[i].currentEmotion = EMOTION_NONE;
            stepFinished = true;
        }
        break;

    case CUTSCENE_MOVE:
        if (action.actionTimer <= deltaTime + 0.001f) {
            int charIdx = -1;
            for(int i=0; i<g_cutscene.characterCount; i++)
                if(g_cutscene.characters[i].name == action.characterName) charIdx = i;

            if(charIdx == -1 && g_cutscene.characterCount < 5) {
                charIdx = g_cutscene.characterCount++;
                g_cutscene.characters[charIdx].name = action.characterName;
            }
            if(charIdx != -1) {
                g_cutscene.characters[charIdx].pos = getNPCPosition(action.characterName);
                g_cutscene.characters[charIdx].targetPos = {action.targetX, action.targetY};
                g_cutscene.characters[charIdx].isMoving = true;
            }
        }
        {
        int idx = -1;
        for(int i=0; i<g_cutscene.characterCount; i++)
            if(g_cutscene.characters[i].name == action.characterName) idx = i;

        if (idx != -1) {
            sf::Vector2f diff = g_cutscene.characters[idx].targetPos - g_cutscene.characters[idx].pos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

            // 🔥 زودنا المسافة لـ 150 عشان اللاعب يتحرك وراه أسرع بكتير
            if (!g_cutscene.characters[idx].isMoving || dist < 450.0f) {
                stepFinished = true;
            }
        }
        }
            break;

    case CUTSCENE_SPEAK:
        if (action.actionTimer <= deltaTime + 0.001f) {
            startDialogue(action.characterName, action.lines, action.lineCount, getNPCAvatar(action.characterName));
        }
        if (!isDialogueActive()) stepFinished = true;
        break;

    case CUTSCENE_CHANGE_MAP:
        // تحديث كل السيستم عشان عمرو يختفي من ماب ويظهر في ماب
        for (int i = 0; i < npcCount; i++) {
            if (allNPCs[i].name == action.characterName) {
                allNPCs[i].currentMap = action.targetMap;
                allNPCs[i].pos = action.spawnPos;
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

        // قللت الـ 150 لـ 20 عشان يوصل للنقطة بالظبط وميقفش بدري أوي
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
        extern bool isFading;

        // الطريقة القديمة السريعة والمضمونة
        worldSetCurrentMap(world, action.targetMap);
        player.pos = action.spawnPos;
        player.sprite.setPosition(player.pos);

        // بنرمي السواد ونخلي الـ Main Loop هي اللي تشيله (Fade In)
        fadeAlpha = 255.0f;
        isFading = true;

        stepFinished = true;
        break;
        }
}

    if (stepFinished) {
        scene.currentActionIdx++;
        if (scene.currentActionIdx < scene.actions.size()) {
            scene.actions[scene.currentActionIdx].actionTimer = 0.f;
        }
    }
}


// 1. تعديل الحسبة عشان الصورة "عمود واحد" مش "Sheet عريضة"
sf::IntRect calculateEmoteRect(Emotion emotion, int frame) {
    if (emotion == EMOTION_NONE) return sf::IntRect(0, 0, 0, 0);

    int emoteIndex = static_cast<int>(emotion);

    // الفريم الواحد عرضه 16
    int rectX = frame * 16;

    // الصف الواحد (الايموت) ارتفاعه 16
    int rectY = emoteIndex * 16;

    return sf::IntRect(rectX, rectY, 16, 16);
}
// 2. تعديل الـ Origin في دالة initCutsceneSystem
void initCutsceneSystem() {
    if (!g_cutscene.emoteSheet.loadFromFile("assets/sprites/emotes/emotes.png")) {
        std::cout << "[ERROR] Could not load emotes.png" << std::endl;
    }
    g_cutscene.emoteSprite.setTexture(g_cutscene.emoteSheet);

    // Origin في نص الـ 16 بكسل ومن تحت
    g_cutscene.emoteSprite.setOrigin(8.f, 16.f);

    // 🔥 تكبير الايموت عشان يبان (لو اللعبة عندك Scale 3x أو 4x)
    // جرب 3.0f أو 4.0f لحد ما تحس حجمه مناسب لراس الشخصية
    g_cutscene.emoteSprite.setScale(3.0f, 3.0f);
}


// 3. تعديل مكان الرسم في drawCutsceneOverlay عشان ميبقاش لازق في الدماغ
void drawCutsceneOverlay(sf::RenderWindow& window, sf::Font& font) {
    if (!g_cutscene.isActive) return;

    for (int i = 0; i < g_cutscene.characterCount; i++) {
        auto& cs = g_cutscene.characters[i];
        if (cs.currentEmotion != EMOTION_NONE) {
            sf::Vector2f pos = getNPCPosition(cs.name);

            g_cutscene.emoteSprite.setTextureRect(calculateEmoteRect(cs.currentEmotion, cs.currentFrame));

            // ارفعها فوق الراس (بما إننا كبرنا السكيل، الـ Offset محتاج يتظبط)
            // -50 أو -60 بكسل مفروض تكون كويسة
            g_cutscene.emoteSprite.setPosition(pos.x, pos.y - 55.f);

            window.draw(g_cutscene.emoteSprite);
        }
    }
}



bool isCutsceneActive() { return g_cutscene.isActive; }