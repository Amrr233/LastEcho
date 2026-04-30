#include "Phase.h"
#include "DialogueManager.h"
#include "inventory.h"
#include "NPC.h"
#include "Cutscene.h"
#include "Player.h"
#include "Data.h"
#include "chest.h"

extern inventory inv;
extern NPC allNPCs[MAX_NPCS];


// ════════════════════════════════════════════════════════════════
// HELPER
//
// makeGroup({ movers... }, completionDist)
//
// كل MoverTarget:
//   { MoverTarget::NPC,    "name", x, y, speed, startDelay }
//   { MoverTarget::PLAYER, "",     x, y, speed, startDelay }
//
// startDelay = كام ثانية من بداية الـ group قبل ما الشخص ده يبدأ يتحرك
//   0.f  → يبدأ فوراً
//   0.8f → يستنى 0.8 ثانية والباقيين ماشيين وبعدين يبدأ
//
// الـ group مش بيخلص غير لما الكل وصل
// ════════════════════════════════════════════════════════════════
static CutsceneAction makeGroup(
    std::vector<MoverTarget> movers,
    float completionDist = 20.f) {
    CutsceneAction a;
    a.type = CUTSCENE_MOVE_GROUP;
    a.moveGroup.movers = std::move(movers);
    a.completionDistance = completionDist;
    a.isStarted = false;
    return a;
}

// ════════════════════════════════════════════════════════════════
void phaseInit(PhaseSystem &ps) {
    ps.currentPhaseIdx = 0;
    for (int i = 0; i < MAX_FLAGS; i++) ps.gameFlags[i] = false;

    ps.allPhases[0].phaseTitle = "Amr's Magical Guitar";
    ps.allPhases[0].quests[0] = {"The lost ID"};
    ps.allPhases[0].quests[1] = {"WELCOME FCIS!"};
    ps.allPhases[0].quests[2] = {"AMR THE GUITARIST"};
    ps.allPhases[0].quests[3] = {"The scattered Strings"};
    ps.allPhases[0].currentQuestIdx = 3;

    ps.allPhases[1].phaseTitle = "The Corruption Begins";
    ps.allPhases[1].quests[0] = {"Explore the campus"};
    ps.allPhases[1].currentQuestIdx = 0;

    ps.allPhases[2].phaseTitle = "The Discovery";
    ps.allPhases[2].quests[0] = {"Find the truth"};
    ps.allPhases[2].currentQuestIdx = 0;

    ps.allPhases[3].phaseTitle = "Betrayal";
    ps.allPhases[3].quests[0] = {"Uncover the betrayal"};
    ps.allPhases[3].currentQuestIdx = 0;

    ps.allPhases[4].phaseTitle = "Hidden Powers";
    ps.allPhases[4].quests[0] = {"Unlock new abilities"};
    ps.allPhases[4].currentQuestIdx = 0;

    ps.allPhases[5].phaseTitle = "Final Climax";
    ps.allPhases[5].quests[0] = {"The final confrontation"};
    ps.allPhases[5].currentQuestIdx = 0;

    ps.allPhases[6].phaseTitle = "Resolution";
    ps.allPhases[6].quests[0] = {"Restore peace"};
    ps.allPhases[6].currentQuestIdx = 0;
}

// ════════════════════════════════════════════════════════════════
void updatePhaseLogic(PhaseSystem &ps, std::string npcName) {
    switch (ps.currentPhaseIdx) {
        case 0: updatePhase0(ps, npcName);
            break;
        case 1: updatePhase1(ps, npcName);
            break;
        case 2: updatePhase2(ps, npcName);
            break;
        case 3: updatePhase3(ps, npcName);
            break;
        case 4: updatePhase4(ps, npcName);
            break;
        case 5: updatePhase5(ps, npcName);
            break;
        case 6: updatePhase6(ps, npcName);
            break;
        default: break;
    }
}

void checkDialogueReward(PhaseSystem &ps) {
    if (ps.pendingItemTexture != "" && !isDialogueActive()) {
        inv.triggerPickupEffect(ps.pendingItemTexture);
        ps.pendingItemTexture = "";
    }
}

// ════════════════════════════════════════════════════════════════
// PHASE 0
// ════════════════════════════════════════════════════════════════
void updatePhase0(PhaseSystem &ps, std::string npcName) {
    if (npcName == "Friend_NPC") {
        if (!ps.gameFlags[0]) {
            std::string lines1[] = {"Oh! Your ID card is here.", "Take it and go to the gate."};
            inv.addItem("id_card", "assets/items/idcard.png");
            ps.pendingItemTexture = "assets/items/idcard.png";
            startDialogue("Friend", lines1, 2, getNPCAvatar("Friend_NPC"));
            ps.gameFlags[0] = true;
            ps.allPhases[0].currentQuestIdx = 1;
        } else {
            std::string lines2[] = {"Go! The security is waiting."};
            startDialogue("Friend", lines2, 1, getNPCAvatar("Friend_NPC"));
        }
    } else if (npcName == "Security_Guard") {
        if (ps.gameFlags[0]) {
            std::string ok[] = {"Valid ID. Welcome to FCIS!", "Go find Amr in the Hallway, he's waiting."};
            startDialogue("Security", ok, 2, getNPCAvatar("Security_Guard"));
            ps.allPhases[0].currentQuestIdx = 2;
        } else {
            std::string no[] = {"No ID, No entry!", "Go find your ID first."};
            startDialogue("Security", no, 2, getNPCAvatar("Security_Guard"));
        }
    } else if (npcName == "amr") {
        if (ps.allPhases[0].currentQuestIdx < 2) {
            std::string wait[] = {"I can't talk now, get your ID first!"};
            startDialogue("Amr", wait, 1, getNPCAvatar("amr"));
        } else if (ps.allPhases[0].currentQuestIdx == 2 && !isCutsceneActive()) {
            std::vector<CutsceneAction> steps;

            // ── Emotions ─────────────────────────────────────────
            CutsceneAction a1;
            a1.type = CUTSCENE_EMOTION;
            a1.characterName = "amr";
            a1.emotion = EMOTION_SURPRISE;
            a1.emotionDuration = 2.0f;
            steps.push_back(a1);

            CutsceneAction a15;
            a15.type = CUTSCENE_EMOTION;
            a15.characterName = "amr";
            a15.emotion = EMOTION_LOVE;
            a15.emotionDuration = 2.0f;
            steps.push_back(a15);


            // ── Dialogue ─────────────────────────────────────────
            CutsceneAction s2;
            s2.type = CUTSCENE_SPEAK;
            s2.characterName = "amr";
            s2.lineCount = 6;
            s2.lines[0] = "So... you can see me.";
            s2.lines[1] = "Good. That means you're not like the others.";
            s2.lines[2] = "This place wasn't always like this.";
            s2.lines[3] = "Something broke and now things are slipping through.";
            s2.lines[4] = "And somehow you got pulled into all of this.";
            s2.lines[5] = "Follow me..";
            steps.push_back(s2);

            // ════════════════════════════════════════════════════
            // MOVEMENTS
            //
            // MoverTarget(type, name, x, y, speed, startDelay)
            //
            // startDelay = كام ثانية من بداية الـ group يستنى قبل يتحرك
            //   amr   startDelay=0   → يبدأ فوراً
            //   player startDelay=0.8 → يستنى 0.8 ثانية وامر ماشي وبعدين يبدأ
            //
            // عشان تغير الـ delay: غير الرقم الأخير في MoverTarget بتاع الـ PLAYER
            // عشان تغير السرعة:   غير الرقم قبل الأخير
            // ════════════════════════════════════════════════════

            // [LOBBY] amr يبدأ فوراً، player يبدأ بعده بـ 0.8 ثانية
            steps.push_back(makeGroup({
                {MoverTarget::NPC, "amr", 1058.85f, 297.645f, 190.f, 0.0f},
                {MoverTarget::PLAYER, "", 900.85f, 297.645f, 180.f, 0.8f}
            }));

            // ── Transition to vertPassage ─────────────────────────
            CutsceneAction s7;
            s7.type = CUTSCENE_CHANGE_MAP;
            s7.characterName = "amr";
            s7.targetMap = "vertPassage";
            s7.spawnPos = {255.067f, 884.582f};
            steps.push_back(s7);

            CutsceneAction s8;
            s8.type = CUTSCENE_PLAYER_TELEPORT;
            s8.targetMap = "vertPassage";
            s8.spawnPos = {255.067f, 884.582f};
            steps.push_back(s8);

            // [vertPassage] amr يبدأ فوراً، player بعده بـ 0.8 ثانية
            steps.push_back(makeGroup({
                {MoverTarget::NPC, "amr", 255.067f, 294.625f, 160.f, 0.0f},
                {MoverTarget::PLAYER, "", 255.067f, 350.0f, 180.f, 0.8f}
            }));

            // [vertPassage] amr يمشي يمين، player بعده بـ 0.8 ثانية
            steps.push_back(makeGroup({
                {MoverTarget::NPC, "amr", 374.044f, 294.625f, 160.f, 0.0f},
                {MoverTarget::PLAYER, "", 335.044f, 340.0f, 180.f, 0.8f}
            }));

            // ── Transition to wcm2 ────────────────────────────────
            CutsceneAction s13;
            s13.type = CUTSCENE_CHANGE_MAP;
            s13.characterName = "amr";
            s13.targetMap = "wcm2";
            s13.spawnPos = {340.0f, 542.879f};
            steps.push_back(s13);

            CutsceneAction s14;
            s14.type = CUTSCENE_PLAYER_TELEPORT;
            s14.targetMap = "wcm2";
            s14.spawnPos = {380.0f, 542.879f};
            steps.push_back(s14);

            // [wcm2] amr يمشي لليسار، player بعده بـ 0.8 ثانية
            steps.push_back(makeGroup({
                {MoverTarget::NPC, "amr", 150.0f, 542.879f, 160.f, 0.0f},
                {MoverTarget::PLAYER, "", 200.0f, 542.879f, 180.f, 0.8f}
            }));

            // ════════════════════════════════════════════════════
            // بعد ما وصل للنقطة اللي إنت عايزها في wcm2
            CutsceneAction lookUp;
            lookUp.type = CUTSCENE_SET_DIRECTION;
            lookUp.characterName = "amr";
            lookUp.direction = 3; // نفترض إن 1 هو North (باصص لفوق)
            steps.push_back(lookUp);
            // ── Ending ───────────────────────────────────────────
            CutsceneAction s16;
            s16.type = CUTSCENE_EMOTION;
            s16.characterName = "amr";
            s16.emotion = EMOTION_SAD;
            s16.emotionDuration = 5.0f;
            steps.push_back(s16);


            CutsceneAction s17;
            s17.type = CUTSCENE_SPEAK;
            s17.characterName = "amr";
            s17.lineCount = 5;
            s17.lines[0] = "I can't fight like this.";
            s17.lines[1] = "My power... it's tied to my guitar.";
            s17.lines[2] = "It was shattered. The strings were scattered.";
            s17.lines[3] = "Find them... all of them.";
            s17.lines[4] = "Then come back to me.";
            steps.push_back(s17);

            steps.push_back(makeGroup({{MoverTarget::NPC, "amr", 150.0f, 470.879f, 160.f, 0.0f}}));
            steps.push_back(makeGroup({{MoverTarget::NPC, "amr", 750.0f, 542.879f, 160.f, 0.0f}}));
            steps.push_back(makeGroup({{MoverTarget::NPC, "amr", 750.0f, 660.879f, 160.f, 0.0f}}));

            CutsceneAction s18;
            s18.type = CUTSCENE_CHANGE_MAP;
            s18.characterName = "amr";
            s18.targetMap = "vertPassage";
            s18.spawnPos = {340.0f, 542.879f};
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
            // لو احنا في كويست 3 ومجمعش الاوتار او جمع الاوتار
        }else if (ps.allPhases[0].currentQuestIdx ==3 && !gameFlags[7]) {
            std::string wait[] = {"You have to help me, please find them..."};
            startDialogue("Amr", wait, 1, getNPCAvatar("amr"));
            gameFlags[7] = true; // بععتبر انه جمع الاوتار
            // gameFlags[8] = false;
        }else if (ps.allPhases[0].currentQuestIdx ==3 && gameFlags[7] && !gameFlags[8]) {
            std::string wait1[] = {"You actually did it?",
              "Impressive.",
              "Now listen carefully...",
              "This curse... it's out of this world.",
              "This guitar is more than just an instrument.",
              "Play it.",
              "Let it choose you."
            };
            inv.addItem("magical_guitar" , "assets/items/magical_guitar.png");
            inv.addItem("note" , "assets/items/note.png");
            gameFlags[8] = true;
        }
        //========================
        else if (npcName == "Key_Keeper") {
            if (ps.allPhases[0].currentQuestIdx < 3) {
                std::string notYet[] = {"I don't know you yet.", "Come back later."};
                startDialogue("Stranger", notYet, 2, getNPCAvatar("Key_Keeper"));
            } else if (!ps.gameFlags[1]) {
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
            } else {
                std::string already[] = {"You already have the key.", "Go find the chest."};
                startDialogue("Keeper", already, 2, getNPCAvatar("Key_Keeper"));
            }
        } else if (npcName == "Receptionist") {
            std::string liness[] = {
                "Morning.",
                "It looks like the perfect day to sit and enjoy my tea.",
                "But remember... not everything in this place is truly like how it seems."
            };
            startDialogue("Receptionist", liness, 3, getNPCAvatar("Receptionist"));
        } else if (npcName == "student") {
            std::string liness[] = {
                "You look new here.",
                "Take care... some doors are better left unopened."
            };
            startDialogue("student1", liness, 2, getNPCAvatar("student"));
        }
    }
}

// ════════════════════════════════════════════════════════════════
// PHASE 1-6: Stubs
// ════════════════════════════════════════════════════════════════
void updatePhase1(PhaseSystem &ps, std::string npcName) {
}

void updatePhase2(PhaseSystem &ps, std::string npcName) {
}

void updatePhase3(PhaseSystem &ps, std::string npcName) {
}

void updatePhase4(PhaseSystem &ps, std::string npcName) {
}

void updatePhase5(PhaseSystem &ps, std::string npcName) {
}

void updatePhase6(PhaseSystem &ps, std::string npcName) {
}
