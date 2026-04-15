#include "DialogueManager.h"
#include "Data.h"
#include <iostream>
using namespace sf;
using namespace std;

Sprite boxSprite;
Texture boxTexture;
Font font;
Text dialogueText;
Text nameText;

bool isOpen = false;

// DIALOGUE LINES SYSTEM
string currentMessages[MAX_DIALOGUE_LINES]; // كل الجمل
int totalLines = 0;      // عدد الجمل الكلي
int currentLineIdx = 0;  // الجملة الحالية

// TYPEWRITER SYSTEM (حرف حرف)
string fullText;     // النص الكامل للجملة
string displayText;   // النص اللي بيظهر تدريجيًا

float typeTimer = 0.f; // عداد وقت
float typeSpeed = 0.03f; // كل قد إيه نضيف حرف
int charIdx = 0;       // رقم الحرف الحالي
// TEXT WRAPPING FUNCTION
// TEXT WRAPPING LIMIT
float maxWidth = 500.f; // أقصى عرض للسطر داخل الصندوق
// الفكرة:
// - نقسم النص لكلمات
// - نحط كلمة كلمة في سطر
// - لو السطر طول → ننقل لسطر جديد
string wrapTextSimple(const string& text) {

    string word = "";   // الكلمة الحالية
    string line = "";   // السطر الحالي
    string result = ""; // النتيجة النهائية

    // loop على كل حرف في النص
    for (int i = 0; i < text.size(); i++) {
        // لو الحرف مش مسافة → نبني الكلمة
        if (text[i] != ' ') {
            word += text[i];
        }
        // لما نوصل لمسافة أو آخر حرف → الكلمة خلصت
        if (text[i] == ' ' || i == text.size() - 1) {
            // نجرب نحط الكلمة في السطر
            string testLine = line + word + " ";
            // نحط النص في SFML عشان نقيس العرض
            dialogueText.setString(testLine);
            // لو السطر أطول من الحد المسموح
            if (dialogueText.getGlobalBounds().width > maxWidth) {
                // ننقل السطر للنتيجة ونبدأ جديد
                result += line + "\n";
                line = word + " ";
            }
            else {
                // نكمل نفس السطر
                line = testLine;
            }
            word = ""; // reset للكلمة
        }
    }
    return result + line;
}

// CENTER TEXT (توسيط النص)
void centerText() {

    // =========================
    // NAME TEXT
    // =========================

    // getLocalBounds():
    // بترجع حجم النص (width/height)
    FloatRect nameBounds = nameText.getLocalBounds();

    // setOrigin():
    // بيحدد نقطة الأصل (pivot)
    nameText.setOrigin(nameBounds.left, nameBounds.top);

    // setPosition():
    // مكان الرسم على الشاشة
    nameText.setPosition(910.f, 485.f);


    // =========================
    // DIALOGUE TEXT
    // =========================

    FloatRect diagBounds = dialogueText.getLocalBounds();

    dialogueText.setOrigin(diagBounds.left, diagBounds.top);

    dialogueText.setPosition(300.f, 320.f);
}


// ==================================================
// INIT DIALOGUE SYSTEM
// ==================================================
void initDialogue() {

    // تحميل صورة الصندوق
    if (!boxTexture.loadFromFile("assets/gameplay/dialogue.png")) {
        cout << "ERROR: Dialogue Box Texture not found!" << endl;
    }

    // ربط الصورة بالـ sprite
    boxSprite.setTexture(boxTexture);

    // تكبير الصورة
    boxSprite.setScale(1.4f, 1.4f);

    // توسيط الصندوق تحت الشاشة
    float x = (SCREEN_W / 2.0f) - (boxSprite.getGlobalBounds().width / 2.0f);
    float y = SCREEN_H - boxSprite.getGlobalBounds().height - 20.0f;

    boxSprite.setPosition(x, y);


    // تحميل الخط
    if (!font.loadFromFile("assets/fonts/pixelsix00.ttf")) {
        cout << "ERROR: Font not found!" << endl;
    }

    // إعداد اسم NPC
    nameText.setFont(font);
    nameText.setCharacterSize(26);
    nameText.setFillColor(Color::Black);

    // إعداد نص الحوار
    dialogueText.setFont(font);
    dialogueText.setCharacterSize(22);
    dialogueText.setFillColor(Color::White);
}


// ==================================================
// START DIALOGUE
// ==================================================
void startDialogue(string name, string messages[], int count) {

    isOpen = true; // فتح الديالوج

    nameText.setString(name); // اسم الـ NPC

    // حماية من overflow
    totalLines = (count > MAX_DIALOGUE_LINES)
        ? MAX_DIALOGUE_LINES
        : count;

    currentLineIdx = 0;

    // نسخ الجمل
    for (int i = 0; i < totalLines; i++) {
        currentMessages[i] = messages[i];
    }

    // أول جملة
    fullText = currentMessages[currentLineIdx];
    displayText = "";

    charIdx = 0;
    typeTimer = 0.f;

    dialogueText.setString("");

    centerText();
}


// ==================================================
// NEXT LINE
// ==================================================
void nextLine() {

    // لو لسه بيكتب → اعرض النص كامل فورًا
    if (charIdx < (int)fullText.length()) {

        charIdx = fullText.length();
        displayText = fullText;

        dialogueText.setString(wrapTextSimple(displayText));
        return;
    }

    // روح للجملة اللي بعدها
    currentLineIdx++;

    if (currentLineIdx < totalLines) {

        fullText = currentMessages[currentLineIdx];

        displayText = "";
        charIdx = 0;
        typeTimer = 0.f;

        dialogueText.setString("");

        centerText();
    }
    else {
        // خلص الحوار
        isOpen = false;
    }
}


// ==================================================
// UPDATE DIALOGUE (typing effect)
// ==================================================
void updateDialogue(float deltaTime) {

    if (!isOpen) return;

    // لسه في حروف
    if (charIdx < (int)fullText.length()) {

        typeTimer += deltaTime;

        // كل typeSpeed نضيف حرف
        if (typeTimer >= typeSpeed) {

            typeTimer = 0.f;

            displayText += fullText[charIdx]; // إضافة حرف واحد
            charIdx++;

            dialogueText.setString(wrapTextSimple(displayText));
        }
    }
}


// ==================================================
// DRAW DIALOGUE
// ==================================================
void drawDialogue(RenderWindow& window) {

    if (isOpen) {
        window.draw(boxSprite);
        window.draw(nameText);
        window.draw(dialogueText);
    }
}


// ==================================================
// CHECK IF DIALOGUE IS ACTIVE
// ==================================================
bool isDialogueActive() {
    return isOpen;
}