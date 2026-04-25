#include "HiddenWordsminigame.h"
#include <iostream>

// 1. بنملا بيانات الفيلم والريفيو
void initReviewGame(MovieReview& review) {
    review.title = "FIGHT CLUB (1999)";
    // حرف الـ \n بيعمل سطر جديد عشان الكلام م يخرجش بره الشاشة
    review.reviewTemplate = "The first rule of [____] is: \nYou do not talk about [____].";
    
    review.solutions = {"FIGHT CLUB", "FIGHT CLUB"}; // الكلمات الصح
    review.hints = {"It's an underground society.", "Repeat the first rule!"};
    
    review.currentWordIdx = 0;
    review.userInput = "";
    review.isCleared = false;
}

// 2. منطق الكتابة والـ Backspace والـ Enter
void updateReviewInput(sf::Event& event, MovieReview& review) {
    if (review.isCleared) return;

    // التعامل مع كتابة الحروف
    if (event.type == sf::Event::TextEntered) {
        // لو داس Backspace (رقم 8 في الـ ASCII) يمسح آخر حرف
        if (event.text.unicode == 8) {
            if (!review.userInput.empty()) {
                review.userInput.pop_back();
            }
        }
        // لو كتب حرف عادي (مش Enter ومش Backspace)
        else if (event.text.unicode < 128 && event.text.unicode != 13) {
            review.userInput += static_cast<char>(event.text.unicode);
        }
    }

    // لو داس Enter يتأكد من الكلمة
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        // بنحول اللي كتبه لـ Capital عشان نقارنه بالحل صح
        string tempInput = review.userInput;
        for (auto & c: tempInput) c = toupper(c);

        if (!tempInput.empty() && tempInput == review.solutions[review.currentWordIdx]) {
            review.currentWordIdx++;
            review.userInput = ""; // نصفر الخانة للكلمة اللي بعدها
            
            if (review.currentWordIdx >= review.solutions.size()) {
                review.isCleared = true;
            }
        }
    }
}

// 3. الرسم فوق الشاشة
void drawReviewGame(sf::RenderWindow& window, sf::Sprite& screenBg, sf::Font& font, MovieReview& review) {
    // 1. رسم الخلفية (التي تم سنطرتها وتصغيرها في الـ main)
    window.draw(screenBg);

    // 2. نجيب إحداثيات السنتر وأبعاد الشاشة الحالية بعد الـ Scale
    sf::Vector2f pos = screenBg.getPosition();
    sf::FloatRect bounds = screenBg.getGlobalBounds();

    // 3. نحسب "نقطة البداية" للكتابة جوه الشاشة (مثلاً من الناحية الشمال فوق شوية)
    // هنستخدم نسبة مئوية من العرض والطول عشان لو صغرتي الشاشة أكتر الكلام يفضل مظبوط
    float startX = pos.x - (bounds.width * 0.4f); // بيبدأ من 40% شمال السنتر
    float startY = pos.y - (bounds.height * 0.35f); // بيبدأ من 35% فوق السنتر

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(18); // صغرنا الـ Size شوية عشان الشاشة صغرت

    // --- رسم عنوان الفيلم ---
    text.setFillColor(sf::Color(255, 128, 0));
    text.setString(review.title);
    text.setPosition(startX, startY);
    window.draw(text);

    // --- رسم نص الريفيو ---
    text.setFillColor(sf::Color(0, 255, 0));
    text.setString(review.reviewTemplate);
    // بينزل مسافة تحت العنوان بناءً على طول الشاشة
    text.setPosition(startX, startY + (bounds.height * 0.15f));
    window.draw(text);

    // --- رسم الـ Hint والـ Input ---
    if (!review.isCleared) {
        string displayStr = "HINT: " + review.hints[review.currentWordIdx] +
                           "\n\n> " + review.userInput + "_";
        text.setString(displayStr);
        // بينزل لتحت أكتر (نص الشاشة تقريباً)
        text.setPosition(startX, pos.y + (bounds.height * 0.1f));
        window.draw(text);
    } else {
        text.setFillColor(sf::Color::White);
        text.setString("ACCESS GRANTED. \nINFO: Go to the Library...");
        text.setPosition(startX, pos.y + (bounds.height * 0.1f));
        window.draw(text);
    }
}