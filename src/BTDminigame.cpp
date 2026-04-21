//
// Created by farah on 4/18/2026.
//
#include "BTDminigame.h"
#include<Data.h>
#include <iostream>

// --- تجهيز شكل شاشة الكمبيوتر في معمل الـ SC Lab ---
void initBinaryGame(BinaryGameData& data) {
    if (!data.terminalFont.loadFromFile("assets/fonts/terminalfont.ttf")) {
        std::cout << "Error: Font not found!" << std::endl;
    }

    data.promptText.setFont(data.terminalFont);
    data.inputText.setFont(data.terminalFont);

    // 1. خليه ياخد مقاس الشاشة بالظبط (افترضي 800x600 أو استخدمي SCREEN_W/H)
    data.overlay.setSize(sf::Vector2f(SCREEN_W, SCREEN_H));
    data.overlay.setPosition(0, 0);
    data.overlay.setFillColor(sf::Color(0, 0, 0, 240));

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
        if (event.text.unicode >= '0' && event.text.unicode <= '9') {
            data.userInput += static_cast<char>(event.text.unicode);
        }
        else if (event.text.unicode == '\b' && !data.userInput.empty()) {
            data.userInput.pop_back();
        }
        else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
            if (!data.userInput.empty()) {
                // لو اللعبة خلصت أصلاً وداس Enter تاني، اقفل الميني جيم
                if (data.completed) {
                    data.active = false;
                    return;
                }

                if (std::stoi(data.userInput) == data.targetDecimal) {
                    // حالة النجاح: بنغير الرسالة ونعلم إنها خلصت بس بنسيب active = true
                    data.statusMessage = "Access granted.\nDecrypting files...\nFind Albert.";
                    data.completed = true;
                    data.userInput = ""; // نمسح المدخلات عشان نعرض جملة الـ Press Enter
                } else {
                    data.statusMessage = "Access denied.\nTry converting the code to something else.";
                    data.userInput = "";
                }
            }
        }
    }
}

void updateBinaryGame(BinaryGameData& data, float deltaTime) {
    if (!data.active) return;

    if (data.completed) {
        data.displayTimer += deltaTime;

        // 1. نبدأ بالجملة الأولى
        std::string finalMsg = "Access granted.";

        // 2. نضيف الجملة الثانية بعد ثانية
        if (data.displayTimer >= 2.5f) {
            finalMsg += "\nDecrypting files...";
        }

        // 3. نضيف الجملة الثالثة بعد ثانيتين
        if (data.displayTimer >= 5.0f) {
            finalMsg += "\nFind Albert.";
            data.messageStep = 2; // عشان نبقى عارفين إننا خلصنا
        }

        // --- التصحيح هنا: نبعت الـ finalMsg مش الـ statusMessage ---
        data.promptText.setString(finalMsg);
        data.promptText.setFillColor(sf::Color::Cyan);


        data.inputText.setString("[Press M to Exit]");
    }
    else {
        // الحالة العادية قبل الحل
        data.promptText.setString(data.statusMessage + "\n\nID: " + data.targetBinary);
        data.inputText.setString("> " + data.userInput + "_");
        data.promptText.setFillColor(sf::Color::Green);
    }
}

// --- رسم كل حاجة في الويندوز ---
void drawBinaryGame(sf::RenderWindow& window, BinaryGameData& data) {
    if (!data.active) return;

    window.draw(data.overlay);    // ارسم الضلمة الأول
    window.draw(data.promptText); // ارسم رسائل السيستم
    window.draw(data.inputText);  // ارسم سهم الكتابة وكلام اللاعب
}