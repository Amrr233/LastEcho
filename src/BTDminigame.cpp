//
// Created by farah on 4/18/2026.
//
#include "BTDminigame.h"
#include <iostream>

// --- تجهيز شكل شاشة الكمبيوتر في معمل الـ SC Lab ---
void initBinaryGame(BinaryGameData& data, sf::Font& font) {
    if (!data.terminalFont.loadFromFile("assets/fonts/terminal_pixel.ttf")) {
        std::cout << "Error: Could not load hacker font!" << std::endl;
    }

    // ربط النصوص بالخط اللي حملناه
    data.promptText.setFont(data.terminalFont);
    data.inputText.setFont(data.terminalFont);
    // الخلفية السودة الشفافة اللي بتغطي الماب
    data.overlay.setSize(sf::Vector2f(800, 600));
    data.overlay.setFillColor(sf::Color(0, 0, 0, 220));

    // إعداد نص السيستم (الرسائل الغامضة)

    data.promptText.setCharacterSize(22);
    data.promptText.setFillColor(sf::Color::Green); // لون أخضر زي شاشات زمان
    data.promptText.setPosition(100, 150);

    // إعداد مكان كتابة الكود (Input)

    data.inputText.setCharacterSize(28);
    data.inputText.setFillColor(sf::Color::White);
    data.inputText.setPosition(100, 300);
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