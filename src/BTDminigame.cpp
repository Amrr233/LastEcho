//
// Created by farah on 4/18/2026.
//
#include "BTDminigame.h"
#include <iostream>

// --- تجهيز شكل شاشة الكمبيوتر في معمل الـ SC Lab ---
void initBinaryGame(BinaryGameData& data) {
    if (!data.terminalFont.loadFromFile("assets/fonts/terminalfont.ttf")) {
        std::cout << "Error: Font not found!" << std::endl;
    }

    data.promptText.setFont(data.terminalFont);
    data.inputText.setFont(data.terminalFont);

    // 1. خليه ياخد مقاس الشاشة بالظبط (افترضي 800x600 أو استخدمي SCREEN_W/H)
    data.overlay.setSize(sf::Vector2f(800.f, 600.f));

    // 2. أهم سطرين: صفرنا الـ Origin والـ Position عشان يبدأ من (0,0) الشاشة
    data.overlay.setOrigin(0.f, 0.f);
    data.overlay.setPosition(0.f, 0.f);

    // 3. اللون (أسود غامق جداً عشان يفصل اللاعب عن الماب)
    data.overlay.setFillColor(sf::Color(0, 0, 0, 250));

    // 4. تظبيط أماكن الكلام (Positions)
    // هنخلي الكلام يبدأ من مسافة 50 بكسل من الشمال عشان ميبقاش لازق في الحافة
    data.promptText.setCharacterSize(24);
    data.promptText.setFillColor(sf::Color::Green);
    data.promptText.setPosition(60.f, 80.f);

    data.inputText.setCharacterSize(30);
    data.inputText.setFillColor(sf::Color::White);
    data.inputText.setPosition(60.f, 450.f); // الـ Input تحت شوية
}

// --- التعامل مع الكيبورد والـ Hacking Logic ---
void handleBinaryInput(BinaryGameData& data, sf::Event& event) {
    if (!data.active) return;

    if (event.type == sf::Event::TextEntered) {
        // لو اللاعب بيكتب أرقام
        if (event.text.unicode >= '0' && event.text.unicode <= '9') {
            data.userInput += static_cast<char>(event.text.unicode);
        }
        // لو بيمسح غلطة (Backspace)
        else if (event.text.unicode == '\b' && !data.userInput.empty()) {
            data.userInput.pop_back();
        }
        // لو داس Enter عشان يسلم الـ Password
        else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
            if (!data.userInput.empty()) {
                // بنحول اللي كتبه لرقم وبنشوفه بيساوي 67 ولا لأ
                if (std::stoi(data.userInput) == data.targetDecimal) {
                    // حالة النجاح: فك التشفير
                    data.statusMessage = "Access granted.\nDecrypting files...\nFind Albert.";
                    data.completed = true;
                    // هنا ممكن نسيبها ثانية قبل ما نقفل عشان يلحق يقرأ الجملة
                    data.active = false;
                } else {
                    // حالة الفشل: تلميح ذكي للاعب
                    data.statusMessage = "Access denied.\nTry converting the code to something else.";
                    data.userInput = ""; // بنصفر اللي كتبه عشان يحاول تاني
                }
            }
        }
    }
}

// --- تحديث الكلام على الشاشة "لحظة بلحظة" ---
void updateBinaryGame(BinaryGameData& data) {
    if (!data.active) return;

    // بنرسم الرسالة الحالية وجنبها كود الـ Binary (ID)
    // الـ \n بتخلي الـ ID ينزل في سطر جديد لشكل أحلى
    data.promptText.setString(data.statusMessage + "\n\nID: " + data.targetBinary);

    // بنرسم سهم الكتابة وجنبه اللي اللاعب بيكتبه والشرطة الوميضة
    data.inputText.setString("> " + data.userInput + "_");
}

// --- رسم كل حاجة في الويندوز ---
void drawBinaryGame(sf::RenderWindow& window, BinaryGameData& data) {
    if (!data.active) return;

    window.draw(data.overlay);    // ارسم الضلمة الأول
    window.draw(data.promptText); // ارسم رسائل السيستم
    window.draw(data.inputText);  // ارسم سهم الكتابة وكلام اللاعب
}