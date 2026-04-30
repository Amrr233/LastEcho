#include "Phase.h"
#include "DialogueManager.h"
#include "inventory.h"
#include "NPC.h"
#include "Cutscene.h"
#include "Player.h"
#include "chest.h"
#include "HintSystem.h"

extern inventory inv;
extern NPC allNPCs[MAX_NPCS];

void phaseInit(PhaseSystem& ps) {
    ps.currentPhaseIdx = 0;
    for (int i = 0; i < MAX_FLAGS; i++) ps.gameFlags[i] = false;
    ps.allPhases[0].phaseTitle = "Amr's Magical Guitar";
    ps.allPhases[0].quests[0] = { "The lost ID" };
    ps.allPhases[0].quests[1] = { "WELCOME FCIS!" };
    ps.allPhases[0].quests[2] = { "AMR THE GUITARIST" };
    ps.allPhases[0].quests[3] = { "The scattered Strings" };
    ps.allPhases[0].currentQuestIdx = 0;

    ps.allPhases[1].phaseTitle = "The Corruption Begins";
    ps.allPhases[1].quests[0] = { "Explore the campus" };
    ps.allPhases[1].currentQuestIdx = 0;
    // TEMP: skip to strings quest
    // ps.gameFlags[0] = true;
    // ps.gameFlags[1] = true;
    // ps.gameFlags[2] = true;
    // ps.allPhases[0].currentQuestIdx = 3;
}

void updatePhaseLogic(PhaseSystem& ps, std::string npcName, HintSystem& hs) {
    int pIdx = ps.currentPhaseIdx;

    if (pIdx == 0) {
        if (npcName == "Friend_NPC") {
            if (!ps.gameFlags[0]) {
                std::string lines1[] = { "Oh! Your ID card is here.", "Take it and go to the gate." };
                inv.addItem("id_card", "assets/items/idcard.png");
                ps.pendingItemTexture = "assets/items/idcard.png";
                startDialogue("Friend", lines1, 2, getNPCAvatar("Friend_NPC"));
                ps.gameFlags[0] = true;
                ps.allPhases[0].currentQuestIdx = 1;
            } else {
                std::string lines2[] = { "Go! The security is waiting." };
                startDialogue("Friend", lines2, 1, getNPCAvatar("Friend_NPC"));
            }
        }
        else if (npcName == "Security_Guard") {
            if (ps.gameFlags[0]) {
                std::string ok[] = { "Valid ID. Welcome to FCIS!", "Go find Amr in the Hallway, he's waiting." };
                startDialogue("Security", ok, 2, getNPCAvatar("Security_Guard"));
                ps.allPhases[0].currentQuestIdx = 2;
            } else {
                std::string no[] = { "No ID, No entry!", "Go find your ID first." };
                startDialogue("Security", no, 2, getNPCAvatar("Security_Guard"));
            }
        }
        else if (npcName == "amr") {
            if (ps.allPhases[0].currentQuestIdx < 2) {
                std::string wait[] = { "I can't talk now, get your ID first!" };
                startDialogue("Amr", wait, 1, getNPCAvatar("amr"));
            }
            else if (ps.allPhases[0].currentQuestIdx == 2 && !isCutsceneActive()) {
                std::vector<CutsceneAction> steps;

                CutsceneAction a1;
                a1.type = CUTSCENE_EMOTION; a1.characterName = "amr";
                a1.emotion = EMOTION_SURPRISE; a1.emotionDuration = 2.0f;
                steps.push_back(a1);

                CutsceneAction a15;
                a15.type = CUTSCENE_EMOTION; a15.characterName = "amr";
                a15.emotion = EMOTION_LOVE; a15.emotionDuration = 2.0f;
                steps.push_back(a15);

                CutsceneAction s2; s2.type = CUTSCENE_SPEAK; s2.characterName = "amr";
                s2.lineCount = 6;
                s2.lines[0] = "So... you can see me.";
                s2.lines[1] = "Good. That means you're not like the others.";
                s2.lines[2] = "This place wasn't always like this.";
                s2.lines[3] = "Something broke and now things are slipping through.";
                s2.lines[4] = "And somehow you got pulled into all of this.";
                s2.lines[5] = "Follow me..";
                steps.push_back(s2);

                CutsceneAction s3; s3.type = CUTSCENE_MOVE; s3.characterName = "amr";
                s3.targetX = 1058.85f; s3.targetY = 297.645f; steps.push_back(s3);

                CutsceneAction s4; s4.type = CUTSCENE_PLAYER_MOVE;
                s4.targetX = 1010.85f; s4.targetY = 297.645f; steps.push_back(s4);

                CutsceneAction s7; s7.type = CUTSCENE_CHANGE_MAP; s7.characterName = "amr";
                s7.targetMap = "vertPassage"; s7.spawnPos = {255.067f, 884.582f}; steps.push_back(s7);

                CutsceneAction s8; s8.type = CUTSCENE_PLAYER_TELEPORT;
                s8.targetMap = "vertPassage"; s8.spawnPos = {255.067f, 884.582f}; steps.push_back(s8);

                CutsceneAction s9; s9.type = CUTSCENE_MOVE; s9.characterName = "amr";
                s9.targetX = 255.067f; s9.targetY = 294.625f; steps.push_back(s9);

                CutsceneAction s10; s10.type = CUTSCENE_PLAYER_MOVE;
                s10.targetX = 255.067f; s10.targetY = 350.0f; steps.push_back(s10);

                CutsceneAction s11; s11.type = CUTSCENE_MOVE; s11.characterName = "amr";
                s11.targetX = 374.044f; s11.targetY = 350.0f; steps.push_back(s11);

                CutsceneAction s12; s12.type = CUTSCENE_PLAYER_MOVE;
                s12.targetX = 295.044f; s12.targetY = 350.0f; steps.push_back(s12);

                CutsceneAction s13; s13.type = CUTSCENE_CHANGE_MAP; s13.characterName = "amr";
                s13.targetMap = "wcm2"; s13.spawnPos = {340.0f, 542.879f}; steps.push_back(s13);

                CutsceneAction s14; s14.type = CUTSCENE_PLAYER_TELEPORT;
                s14.targetMap = "wcm2"; s14.spawnPos = {380.0f, 542.879f}; steps.push_back(s14);

                CutsceneAction moveUpAmr; moveUpAmr.type = CUTSCENE_MOVE; moveUpAmr.characterName = "amr";
                moveUpAmr.targetX = 150.0f; moveUpAmr.targetY = 542.879f; steps.push_back(moveUpAmr);

                CutsceneAction moveUpPlayer; moveUpPlayer.type = CUTSCENE_PLAYER_MOVE;
                moveUpPlayer.targetX = 200.0f; moveUpPlayer.targetY = 542.879f; steps.push_back(moveUpPlayer);

                CutsceneAction s16;
                s16.type = CUTSCENE_EMOTION; s16.characterName = "amr";
                s16.emotion = EMOTION_SAD; s16.emotionDuration = 5.0f; steps.push_back(s16);

                CutsceneAction s17; s17.type = CUTSCENE_SPEAK; s17.characterName = "amr";
                s17.lineCount = 5;
                s17.lines[0] = "I can't fight like this.";
                s17.lines[1] = "My power... it's tied to my guitar.";
                s17.lines[2] = "It was shattered. The strings were scattered.";
                s17.lines[3] = "Find them... all of them.";
                s17.lines[4] = "Then come back to me.";
                steps.push_back(s17);

                startGenericCutscene("amr_first_meeting", steps);

                for (int i = 0; i < MAX_NPCS; i++) {
                    if (allNPCs[i].name == "amr") {
                        allNPCs[i].waypointsCount = 0;
                        allNPCs[i].isStatic = true;
                        break;
                    }
                }
                ps.allPhases[0].currentQuestIdx = 3;
            }
        }
        else if (npcName == "Key_Keeper") {
            if (ps.allPhases[0].currentQuestIdx < 3) {
                std::string notYet[] = { "I don't know you yet.", "Come back later." };
                startDialogue("Stranger", notYet, 2, getNPCAvatar("Key_Keeper"));
            }
            else if (!ps.gameFlags[1]) {
                std::string giveKey[] = {
                    "So... Amr sent you.",
                    "I've been keeping this safe for a long time.",
                    "Take it. The chest is somewhere in this building.",
                    "Find the strings inside. Amr needs them."
                };
                inv.addItem("key", "assets/sprites/items/key.png");
                ps.pendingItemTexture = "assets/items/key.png";
                player.hasChestKey = false;
                ps.gameFlags[1] = true;
                startDialogue("Keeper", giveKey, 4, getNPCAvatar("Key_Keeper"));
            }
            else {
                std::string already[] = { "You already have the key.", "Go find the chest." };
                startDialogue("Keeper", already, 2, getNPCAvatar("Key_Keeper"));
            }
        }
        else if (npcName == "Gardener") {
            if (ps.allPhases[0].currentQuestIdx < 3) {
                std::string notYet[] = { "..." };
                startDialogue("Gardener", notYet, 1, getNPCAvatar("Gardener"));
            }
            else if (!ps.gameFlags[2]) {
                std::string lines[] = {
                    "Ah... you're looking for something.",
                    "This place holds more secrets than lectures.",
                    "Listen carefully, I'll only say this once.",
                    "I hold what falls but never keep it,",
                    "look beneath where water sleeps.",
                    "Thousands sat upon my back,",
                    "but no one checked beneath my legs.",
                    "I'm here to stop what burns,",
                    "but something golden hides behind me.",
                    "I show you music frozen in time,",
                    "what you seek rests behind my frame.",
                    "Now go. The strings are waiting. Check your notes with [Q] to see my riddles."
                };
                startDialogue("Gardener", lines, 12, getNPCAvatar("Gardener"));
                ps.gameFlags[2] = true;
                unlockHint(hs, 0);
            }
            else {
                std::string reminder[] = {
                    "You still haven't found them all?",
                    "The strings won't find themselves..."
                };
                startDialogue("Gardener", reminder, 2, getNPCAvatar("Gardener"));
            }
        }
        else if (npcName == "Receptionist") {
            std::string liness[] = {
                "Morning.",
                "It looks like the perfect day to sit and enjoy my tea.",
                "But remember... not everything in this place is truly like how it seems."
            };
            startDialogue("Receptionist", liness, 3, getNPCAvatar("Receptionist"));
        }
        else if (npcName == "student") {
            std::string liness[] = {
                "You look new here.",
                "Take care... some doors are better left unopened."
            };
            startDialogue("student1", liness, 2, getNPCAvatar("student"));
        }
    }
}

void checkDialogueReward(PhaseSystem& ps) {
    if (ps.pendingItemTexture != "" && !isDialogueActive()) {
        inv.triggerPickupEffect(ps.pendingItemTexture);
        ps.pendingItemTexture = "";
    }
}

void updateStrings(PhaseSystem& ps, sf::Vector2f playerPos, std::string currentMap) {
    if (ps.allPhases[0].currentQuestIdx < 3) return;
    if (!ps.gameFlags[2]) return;

    if (!ps.gameFlags[3] && currentMap == "wcw") {
        sf::Vector2f pos(490.f, 385.f);
        float dist = std::sqrt(std::pow(playerPos.x-pos.x,2)+std::pow(playerPos.y-pos.y,2));
        if (dist < 50.f && !isDialogueActive()) {
            ps.gameFlags[3] = true; player.stringsCollected++;
            inv.addItem("guitar_string","assets/sprites/items/guitar_string.png");
            inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");
            std::string lines[] = {"You found a guitar string!","3 more to go..."};
            startDialogue("String",lines,2,getNPCAvatar("Gardener"));
        }
    }
    if (!ps.gameFlags[4] && currentMap == "hallAfter") {
        sf::Vector2f pos(980.82f,110.652f);
        float dist = std::sqrt(std::pow(playerPos.x-pos.x,2)+std::pow(playerPos.y-pos.y,2));
        if (dist < 50.f && !isDialogueActive()) {
            ps.gameFlags[4] = true; player.stringsCollected++;
            inv.addItem("guitar_string","assets/sprites/items/guitar_string.png");
            inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");
            std::string lines[] = {"You found a guitar string!","2 more to go..."};
            startDialogue("String",lines,2,getNPCAvatar("Gardener"));
        }
    }
    if (!ps.gameFlags[5] && currentMap == "connHall") {
        sf::Vector2f pos(938.f,253.f);
        float dist = std::sqrt(std::pow(playerPos.x-pos.x,2)+std::pow(playerPos.y-pos.y,2));
        if (dist < 50.f && !isDialogueActive()) {
            ps.gameFlags[5] = true; player.stringsCollected++;
            inv.addItem("guitar_string","assets/sprites/items/guitar_string.png");
            inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");
            std::string lines[] = {"You found a guitar string!","1 more to go..."};
            startDialogue("String",lines,2,getNPCAvatar("Gardener"));
        }
    }
    if (!ps.gameFlags[6] && currentMap == "leftPassage") {
        sf::Vector2f pos(330.f,220.f);
        float dist = std::sqrt(std::pow(playerPos.x-pos.x,2)+std::pow(playerPos.y-pos.y,2));
        if (dist < 50.f && !isDialogueActive()) {
            ps.gameFlags[6] = true; player.stringsCollected++;
            inv.addItem("guitar_string","assets/sprites/items/guitar_string.png");
            inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");
            std::string lines[] = {"You found the last string!","Go back to Amr!"};
            startDialogue("String",lines,2,getNPCAvatar("Gardener"));
            ps.gameFlags[7] = true; player.stringsCollected = 7;
        }
    }
}

void drawStrings(sf::RenderWindow& window, PhaseSystem& ps, std::string currentMap) {
    static sf::Texture extTex; static sf::Sprite extSprite; static bool extLoaded = false;
    if (!extLoaded) {
        extTex.loadFromFile("assets/sprites/items/extinguisher.png");
        extSprite.setTexture(extTex); extSprite.setScale(0.138f,0.138f); extLoaded = true;
    }

    if (!ps.gameFlags[3] && currentMap == "wcw") {
        static sf::Texture sinkTex; static sf::Sprite sinkSprite; static bool sinkLoaded = false;
        if (!sinkLoaded) {
            sinkTex.loadFromFile("assets/sprites/items/pelvis.png");
            sinkSprite.setTexture(sinkTex); sinkSprite.setScale(0.81f,0.86f); sinkLoaded = true;
        }
        sinkSprite.setPosition(192.f,131.f); window.draw(sinkSprite);
    }

    if (currentMap == "hallAfter") {
        static sf::Texture chairStringTex; static sf::Sprite chairStringSprite; static bool chairStringLoaded = false;
        static sf::Texture chairTex;       static sf::Sprite chairSprite;       static bool chairLoaded = false;
        if (!chairStringLoaded) {
            chairStringTex.loadFromFile("assets/sprites/items/chair_string.png");
            chairStringSprite.setTexture(chairStringTex); chairStringSprite.setScale(0.3f,0.3f); chairStringLoaded = true;
        }
        if (!chairLoaded) {
            chairTex.loadFromFile("assets/sprites/items/chair.png");
            chairSprite.setTexture(chairTex); chairSprite.setScale(1.1f,1.1f); chairLoaded = true;
        }
        if (!ps.gameFlags[4]) { chairStringSprite.setPosition(980.82f,110.652f); window.draw(chairStringSprite); }
        else                   { chairSprite.setPosition(985.82f,115.652f);       window.draw(chairSprite); }
    }

    if (!ps.gameFlags[5] && currentMap == "connHall") {
        extSprite.setPosition(938.f,253.f); window.draw(extSprite);
    }

    if (!ps.gameFlags[6] && currentMap == "leftPassage") {
        static sf::Texture paintingTex; static sf::Sprite paintingSprite; static bool paintingLoaded = false;
        if (!paintingLoaded) {
            paintingTex.loadFromFile("assets/sprites/items/painting_string.png");
            paintingSprite.setTexture(paintingTex); paintingSprite.setScale(0.25f,0.3f); paintingLoaded = true;
        }
        paintingSprite.setPosition(330.f,220.f); window.draw(paintingSprite);
    }
}

bool canPickupString(PhaseSystem& ps, sf::Vector2f playerPos, std::string currentMap, HintSystem& hs) {
    if (ps.allPhases[0].currentQuestIdx < 3) return false;
    if (!ps.gameFlags[2]) return false;

    struct StringData { int flag; std::string map; sf::Vector2f pos; float radius; };
    StringData strings[] = {
        {3, "wcw",         {490.f,   385.f},    80.f},
        {4, "hallAfter",   {980.82f, 110.652f}, 80.f},
        {5, "connHall",    {938.f,   253.f},    80.f},
        {6, "leftPassage", {330.f,   220.f},    120.f}
    };
    // only allow pickup of current active string based on hintsUnlocked
    int activeIdx = hs.hintsUnlocked - 1;
    if (activeIdx < 0 || activeIdx > 3) return false;

    auto& s = strings[activeIdx];
    if (ps.gameFlags[s.flag]) return false;
    if (currentMap != s.map) return false;
    float dist = std::sqrt(std::pow(playerPos.x-s.pos.x,2)+std::pow(playerPos.y-s.pos.y,2));
    return dist < s.radius;
}

void pickupString(PhaseSystem& ps, sf::Vector2f playerPos, std::string currentMap, HintSystem& hs) {
    struct StringData { int flag; std::string map; sf::Vector2f pos; float radius; int hintIndex; };
    StringData strings[] = {
        {3, "wcw",         {490.f,   385.f},    80.f,  0},
        {4, "hallAfter",   {980.82f, 110.652f}, 80.f,  1},
        {5, "connHall",    {938.f,   253.f},    80.f,  2},
        {6, "leftPassage", {330.f,   220.f},    120.f, 3}
    };

    int activeIdx = hs.hintsUnlocked - 1;
    if (activeIdx < 0 || activeIdx > 3) return;

    auto& s = strings[activeIdx];
    if (ps.gameFlags[s.flag]) return;
    if (currentMap != s.map) return;

    float dist = std::sqrt(std::pow(playerPos.x-s.pos.x,2)+std::pow(playerPos.y-s.pos.y,2));
    if (dist < s.radius) {
        // ... rest of pickup logic unchanged
        ps.gameFlags[s.flag] = true;
        player.stringsCollected++;

        if (hs.hintsUnlocked < 4) {
            hs.hintsUnlocked++;
            hs.hasNewHint = true;
        }

        inv.addItem("guitar_string","assets/sprites/items/guitar_string.png");
        inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");

        std::string lines[] = { "You found a guitar string!", "Keep searching." };
        startDialogue("String", lines, 2, getNPCAvatar("Gardener"));

        if (player.stringsCollected >= 4) {
            ps.gameFlags[7] = true;
            markAllStringsCollected(hs);
        }
        return;
    }
}