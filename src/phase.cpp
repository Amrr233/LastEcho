#include "Phase.h"
#include "DialogueManager.h"
#include "inventory.h"
#include "NPC.h"
#include "Cutscene.h"
#include "Player.h"
#include "Data.h"
#include "chest.h"
#include "HintSystem.h"

extern inventory inv;
extern NPC allNPCs[MAX_NPCS];
extern bool gameFlags[100];

// ════════════════════════════════════════════════════════════════
// HELPER - makeGroup
//
// MoverTarget(type, name, x, y, speed, startDelay)
//   startDelay=0   → يبدأ فوراً
//   startDelay=0.8 → يستنى 0.8 ثانية والتاني ماشي وبعدين يبدأ
// ════════════════════════════════════════════════════════════════
static CutsceneAction makeGroup(
    std::vector<MoverTarget> movers,
    float completionDist = 20.f)
{
    CutsceneAction a;
    a.type               = CUTSCENE_MOVE_GROUP;
    a.moveGroup.movers   = std::move(movers);
    a.completionDistance = completionDist;
    a.isStarted          = false;
    return a;
}

// ════════════════════════════════════════════════════════════════
// INIT
// ════════════════════════════════════════════════════════════════
void phaseInit(PhaseSystem& ps) {
    ps.currentPhaseIdx = 0;
    for (int i = 0; i < MAX_FLAGS; i++) ps.gameFlags[i] = false;

    ps.allPhases[0].phaseTitle      = "Amr's Magical Guitar";
    ps.allPhases[0].quests[0]       = { "The lost ID" };
    ps.allPhases[0].quests[1]       = { "WELCOME FCIS!" };
    ps.allPhases[0].quests[2]       = { "AMR THE GUITARIST" };
    ps.allPhases[0].quests[3]       = { "The scattered Strings" };
    ps.allPhases[0].quests[4]       = { "Play it..." };
    ps.allPhases[0].currentQuestIdx = 0;

    ps.allPhases[1].phaseTitle      = "The Corruption Begins";
    ps.allPhases[1].quests[0]       = { "Explore the campus" };
    ps.allPhases[1].currentQuestIdx = 0;

    ps.allPhases[2].phaseTitle      = "The Discovery";
    ps.allPhases[2].quests[0]       = { "Find the truth" };
    ps.allPhases[2].currentQuestIdx = 0;

    ps.allPhases[3].phaseTitle      = "Betrayal";
    ps.allPhases[3].quests[0]       = { "Uncover the betrayal" };
    ps.allPhases[3].currentQuestIdx = 0;

    ps.allPhases[4].phaseTitle      = "Hidden Powers";
    ps.allPhases[4].quests[0]       = { "Unlock new abilities" };
    ps.allPhases[4].currentQuestIdx = 0;

    ps.allPhases[5].phaseTitle      = "Final Climax";
    ps.allPhases[5].quests[0]       = { "The final confrontation" };
    ps.allPhases[5].currentQuestIdx = 0;

    ps.allPhases[6].phaseTitle      = "Resolution";
    ps.allPhases[6].quests[0]       = { "Restore peace" };
    ps.allPhases[6].currentQuestIdx = 0;
}

// ════════════════════════════════════════════════════════════════
// DISPATCHER
// ════════════════════════════════════════════════════════════════
void updatePhaseLogic(PhaseSystem& ps, std::string npcName, HintSystem& hs) {
    switch (ps.currentPhaseIdx) {
        case 0: updatePhase0(ps, npcName, hs); break;
        case 1: updatePhase1(ps, npcName, hs); break;
        case 2: updatePhase2(ps, npcName, hs); break;
        case 3: updatePhase3(ps, npcName, hs); break;
        case 4: updatePhase4(ps, npcName, hs); break;
        case 5: updatePhase5(ps, npcName, hs); break;
        case 6: updatePhase6(ps, npcName, hs); break;
        default: break;
    }
}

void checkDialogueReward(PhaseSystem& ps) {
    if (ps.pendingItemTexture != "" && !isDialogueActive()) {
        inv.triggerPickupEffect(ps.pendingItemTexture);
        ps.pendingItemTexture = "";
    }
}

// ════════════════════════════════════════════════════════════════
// STRINGS SYSTEM
// ════════════════════════════════════════════════════════════════
struct StringData {
    int          flag;
    std::string  map;
    sf::Vector2f pos;
    float        radius;
    int          hintIndex;
};

static StringData strings[] = {
    { 3, "wcw",         { 490.f,    385.f    }, 80.f,  0 },
    { 4, "hallAfter",   { 980.82f,  110.652f }, 80.f,  1 },
    { 5, "connHall",    { 938.f,    253.f    }, 80.f,  2 },
    { 6, "leftPassage", { 330.f,    220.f    }, 120.f, 3 },
};

bool canPickupString(PhaseSystem& ps, sf::Vector2f playerPos,
                     std::string currentMap, HintSystem& hs)
{
    if (ps.allPhases[0].currentQuestIdx < 3) return false;
    if (!ps.gameFlags[2]) return false;

    int activeIdx = hs.hintsUnlocked - 1;
    if (activeIdx < 0 || activeIdx > 3) return false;

    auto& s = strings[activeIdx];
    if (ps.gameFlags[s.flag])    return false;
    if (currentMap != s.map)     return false;

    float dist = std::sqrt(std::pow(playerPos.x - s.pos.x, 2) +
                           std::pow(playerPos.y - s.pos.y, 2));
    return dist < s.radius;
}

void pickupString(PhaseSystem& ps, sf::Vector2f playerPos,
                  std::string currentMap, HintSystem& hs)
{
    int activeIdx = hs.hintsUnlocked - 1;
    if (activeIdx < 0 || activeIdx > 3) return;

    auto& s = strings[activeIdx];
    if (ps.gameFlags[s.flag]) return;
    if (currentMap != s.map)  return;

    float dist = std::sqrt(std::pow(playerPos.x - s.pos.x, 2) +
                           std::pow(playerPos.y - s.pos.y, 2));
    if (dist >= s.radius) return;

    ps.gameFlags[s.flag] = true;
    player.stringsCollected++;

    if (hs.hintsUnlocked < 4) {
        hs.hintsUnlocked++;
        hs.hasNewHint = true;
    }

    inv.addItem("guitar_string", "assets/sprites/items/guitar_string.png");
    inv.triggerPickupEffect("assets/sprites/items/guitar_string.png");

    std::string lines[] = { "You found a guitar string!", "Keep searching." };
    startDialogue("String", lines, 2, getNPCAvatar("Gardener"));

    if (player.stringsCollected >= 4) {
        ps.gameFlags[7] = true;
        markAllStringsCollected(hs);
    }
}

void drawStrings(sf::RenderWindow& window, PhaseSystem& ps, std::string currentMap) {
    static sf::Texture extTex;
    static sf::Sprite  extSprite;
    static bool        extLoaded = false;
    if (!extLoaded) {
        extTex.loadFromFile("assets/sprites/items/extinguisher.png");
        extSprite.setTexture(extTex);
        extSprite.setScale(0.138f, 0.138f);
        extLoaded = true;
    }

    // String 1 — wcw (sink)
    if (!ps.gameFlags[3] && currentMap == "wcw") {
        static sf::Texture sinkTex; static sf::Sprite sinkSprite; static bool sinkLoaded = false;
        if (!sinkLoaded) {
            sinkTex.loadFromFile("assets/sprites/items/pelvis.png");
            sinkSprite.setTexture(sinkTex); sinkSprite.setScale(0.81f, 0.86f); sinkLoaded = true;
        }
        sinkSprite.setPosition(192.f, 131.f);
        window.draw(sinkSprite);
    }

    // String 2 — hallAfter (chair)
    if (currentMap == "hallAfter") {
        static sf::Texture chairStringTex; static sf::Sprite chairStringSprite; static bool chairStringLoaded = false;
        static sf::Texture chairTex;       static sf::Sprite chairSprite;       static bool chairLoaded = false;
        if (!chairStringLoaded) {
            chairStringTex.loadFromFile("assets/sprites/items/chair_string.png");
            chairStringSprite.setTexture(chairStringTex); chairStringSprite.setScale(0.3f, 0.3f); chairStringLoaded = true;
        }
        if (!chairLoaded) {
            chairTex.loadFromFile("assets/sprites/items/chair.png");
            chairSprite.setTexture(chairTex); chairSprite.setScale(1.1f, 1.1f); chairLoaded = true;
        }
        if (!ps.gameFlags[4]) { chairStringSprite.setPosition(980.82f, 110.652f); window.draw(chairStringSprite); }
        else                   { chairSprite.setPosition(985.82f, 115.652f);       window.draw(chairSprite); }
    }

    // String 3 — connHall (extinguisher)
    if (!ps.gameFlags[5] && currentMap == "connHall") {
        extSprite.setPosition(938.f, 253.f);
        window.draw(extSprite);
    }

    // String 4 — leftPassage (painting)
    if (!ps.gameFlags[6] && currentMap == "leftPassage") {
        static sf::Texture paintingTex; static sf::Sprite paintingSprite; static bool paintingLoaded = false;
        if (!paintingLoaded) {
            paintingTex.loadFromFile("assets/sprites/items/painting_string.png");
            paintingSprite.setTexture(paintingTex); paintingSprite.setScale(0.25f, 0.3f); paintingLoaded = true;
        }
        paintingSprite.setPosition(330.f, 220.f);
        window.draw(paintingSprite);
    }
}

// ════════════════════════════════════════════════════════════════
// PHASE 0
// ════════════════════════════════════════════════════════════════
void updatePhase0(PhaseSystem& ps, std::string npcName, HintSystem& hs) {

    // ── Friend_NPC ───────────────────────────────────────────────
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

    // ── Security_Guard ───────────────────────────────────────────
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

    // ── Amr ──────────────────────────────────────────────────────
    else if (npcName == "amr") {
        if (ps.allPhases[0].currentQuestIdx < 2) {
            std::string wait[] = { "I can't talk now, get your ID first!" };
            startDialogue("Amr", wait, 1, getNPCAvatar("amr"));
        }
        else if (ps.allPhases[0].currentQuestIdx == 2 && !isCutsceneActive()) {
            std::vector<CutsceneAction> steps;

            // Emotions
            CutsceneAction a1;
            a1.type = CUTSCENE_EMOTION; a1.characterName = "amr";
            a1.emotion = EMOTION_SURPRISE; a1.emotionDuration = 2.0f;
            steps.push_back(a1);

            CutsceneAction a15;
            a15.type = CUTSCENE_EMOTION; a15.characterName = "amr";
            a15.emotion = EMOTION_LOVE; a15.emotionDuration = 2.0f;
            steps.push_back(a15);

            // Dialogue
            CutsceneAction s2;
            s2.type = CUTSCENE_SPEAK; s2.characterName = "amr";
            s2.lineCount = 6;
            s2.lines[0] = "So... you can see me.";
            s2.lines[1] = "Good. That means you're not like the others.";
            s2.lines[2] = "This place wasn't always like this.";
            s2.lines[3] = "Something broke and now things are slipping through.";
            s2.lines[4] = "And somehow you got pulled into all of this.";
            s2.lines[5] = "Follow me..";
            steps.push_back(s2);

            // [LOBBY] amr يبدأ فوراً، player بعده بـ 0.8 ثانية
            steps.push_back(makeGroup({
                { MoverTarget::NPC,    "amr", 1058.85f, 297.645f, 190.f, 0.0f },
                { MoverTarget::PLAYER, "",     900.85f, 297.645f, 180.f, 0.8f }
            }));

            // Transition → vertPassage
            CutsceneAction s7;
            s7.type = CUTSCENE_CHANGE_MAP; s7.characterName = "amr";
            s7.targetMap = "vertPassage"; s7.spawnPos = { 255.067f, 884.582f };
            steps.push_back(s7);

            CutsceneAction s8;
            s8.type = CUTSCENE_PLAYER_TELEPORT;
            s8.targetMap = "vertPassage"; s8.spawnPos = { 255.067f, 884.582f };
            steps.push_back(s8);

            // [vertPassage] لفوق
            steps.push_back(makeGroup({
                { MoverTarget::NPC,    "amr", 255.067f, 294.625f, 160.f, 0.0f },
                { MoverTarget::PLAYER, "",    255.067f, 350.0f,   180.f, 0.8f }
            }));

            // [vertPassage] يمين
            steps.push_back(makeGroup({
                { MoverTarget::NPC,    "amr", 374.044f, 294.625f, 160.f, 0.0f },
                { MoverTarget::PLAYER, "",    335.044f, 340.0f,   180.f, 0.8f }
            }));

            // Transition → wcm2
            CutsceneAction s13;
            s13.type = CUTSCENE_CHANGE_MAP; s13.characterName = "amr";
            s13.targetMap = "wcm2"; s13.spawnPos = { 340.0f, 542.879f };
            steps.push_back(s13);

            CutsceneAction s14;
            s14.type = CUTSCENE_PLAYER_TELEPORT;
            s14.targetMap = "wcm2"; s14.spawnPos = { 380.0f, 542.879f };
            steps.push_back(s14);

            // [wcm2] لليسار
            steps.push_back(makeGroup({
                { MoverTarget::NPC,    "amr", 150.0f, 542.879f, 160.f, 0.0f },
                { MoverTarget::PLAYER, "",    200.0f, 542.879f, 180.f, 0.8f }
            }));

            // Ending emotion + dialogue
            CutsceneAction s16;
            s16.type = CUTSCENE_EMOTION; s16.characterName = "amr";
            s16.emotion = EMOTION_SAD; s16.emotionDuration = 5.0f;
            steps.push_back(s16);

            CutsceneAction s17;
            s17.type = CUTSCENE_SPEAK; s17.characterName = "amr";
            s17.lineCount = 5;
            s17.lines[0] = "I can't fight like this.";
            s17.lines[1] = "My power... it's tied to my guitar.";
            s17.lines[2] = "It was shattered. The strings were scattered.";
            s17.lines[3] = "Find them... all of them.";
            s17.lines[4] = "Then come back to me.";
            steps.push_back(s17);

            // amr يمشي ويختفي
            steps.push_back(makeGroup({ { MoverTarget::NPC, "amr", 150.0f,  470.879f, 160.f, 0.0f } }));
            steps.push_back(makeGroup({ { MoverTarget::NPC, "amr", 750.0f,  542.879f, 160.f, 0.0f } }));
            steps.push_back(makeGroup({ { MoverTarget::NPC, "amr", 750.0f,  660.879f, 160.f, 0.0f } }));

            CutsceneAction s18;
            s18.type = CUTSCENE_CHANGE_MAP; s18.characterName = "amr";
            s18.targetMap = "vertPassage"; s18.spawnPos = { 340.0f, 542.879f };
            steps.push_back(s18);

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
        else if (ps.allPhases[0].currentQuestIdx == 3 && !ps.gameFlags[7]) {
            std::string wait[] = { "You have to help me, please find them..." };
            startDialogue("Amr", wait, 1, getNPCAvatar("amr"));
        }
        else if (ps.allPhases[0].currentQuestIdx == 3 && ps.gameFlags[7]) {
            std::string wait2[] = {
                "You actually did it?",
                "Impressive.",
                "Now listen carefully...",
                "This curse... it's out of this world.",
                "This guitar is more than just an instrument.",
                "Play it.",
                "Let it choose you."
            };
            startDialogue("Amr", wait2, 7, getNPCAvatar("amr"));
            inv.addItem("magical_guitar", "assets/items/magical_guitar.png");
            inv.addItem("note",           "assets/items/note.png");
            ps.allPhases[0].currentQuestIdx = 4;
        }
    }

    // ── Key_Keeper ───────────────────────────────────────────────
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
            ps.gameFlags[1] = true;
            startDialogue("Keeper", giveKey, 4, getNPCAvatar("Key_Keeper"));
        }
        else {
            std::string already[] = { "You already have the key.", "Go find the chest." };
            startDialogue("Keeper", already, 2, getNPCAvatar("Key_Keeper"));
        }
    }

    // ── Gardener ─────────────────────────────────────────────────
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

    // ── Receptionist ─────────────────────────────────────────────
    else if (npcName == "Receptionist") {
        std::string liness[] = {
            "Morning.",
            "It looks like the perfect day to sit and enjoy my tea.",
            "But remember... not everything in this place is truly like how it seems."
        };
        startDialogue("Receptionist", liness, 3, getNPCAvatar("Receptionist"));
    }

    // ── student ──────────────────────────────────────────────────
    else if (npcName == "student") {
        std::string liness[] = {
            "You look new here.",
            "Take care... some doors are better left unopened."
        };
        startDialogue("student1", liness, 2, getNPCAvatar("student"));
    }
}

// ════════════════════════════════════════════════════════════════
// PHASE 1-6: Stubs
// ════════════════════════════════════════════════════════════════
void updatePhase1(PhaseSystem& ps, std::string npcName, HintSystem& hs) { }
void updatePhase2(PhaseSystem& ps, std::string npcName, HintSystem& hs) { }
void updatePhase3(PhaseSystem& ps, std::string npcName, HintSystem& hs) { }
void updatePhase4(PhaseSystem& ps, std::string npcName, HintSystem& hs) { }
void updatePhase5(PhaseSystem& ps, std::string npcName, HintSystem& hs) { }
void updatePhase6(PhaseSystem& ps, std::string npcName, HintSystem& hs) { }