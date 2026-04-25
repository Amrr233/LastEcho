#include "HiddenWordsminigame.h"
#include <iostream>

// 1. بنملا بيانات الفيلم والريفيو
void initReviewGame(MovieReview& review) {
    review.title = "    The Hidden Secret (2013)";

    // الريفيو مع الكلمات الناقصة (مكانها ____)
    review.reviewTemplate =
        "This doesn't feel like a story anymore... it feels like something breaking through\n"
        "reality itself.\n\n"
        "Every scene pulls you deeper, as if the world is forming a ____between what\n"
        "is real and what is not.\n\n"
        "At some point, meaning collapses completely, and everything starts pointing\n"
        "toward what lies beyond.\n\n"
        "And what's ______starts getting revealed through every passing moment,\n"
        "as if it was always there waiting for its time to shine.\n\n"
        "Leaving you overwhelmed by how life changes, like something has already\n"
        "choen where you'll end up and there's no chance to run away.\n";


    // الكلمات المفقودة
    review.solutions.push_back("gateway");
    review.solutions.push_back("unknown");

    // الـ Hints (عربي وانجلش عشان نسهلها)
    review.hints.push_back("A path or entrance to another place");
    review.hints.push_back("Something not identified or familiar");

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
    window.draw(screenBg);

    sf::Vector2f pos = screenBg.getPosition();
    sf::FloatRect bounds = screenBg.getGlobalBounds();
    sf::Text text;
    text.setFont(font);

    // 1. حالة الفوز - ليها الأولوية الأولى
    if (review.isCleared) {
        text.setCharacterSize(22);
        text.setFillColor(sf::Color::Cyan);
        string winMessage = "ACCESS GRANTED.\n\n\"A GATEWAY TO THE UNKNOWN\"";
        text.setString(winMessage);

        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        text.setPosition(pos.x, pos.y);
        window.draw(text);

        return; // اخرج عشان ميرسمش حاجة تانية فوقيها
    }

    // 2. حالة وجود خطأ
    if (review.errorMessage != "") {
        text.setFillColor(sf::Color::Red);
        text.setCharacterSize(20);
        text.setString(review.errorMessage);
        sf::FloatRect textBounds = text.getLocalBounds();
        text.setOrigin(textBounds.width / 2.0f, textBounds.height / 2.0f);
        text.setPosition(pos.x, pos.y);
        window.draw(text);
    }
    // 3. حالة اللعب العادية
    else {
        float startX = pos.x - (bounds.width * 0.41f);
        float startY = pos.y - (bounds.height * 0.40f);

        // رسم العنوان
        text.setCharacterSize(15);
        text.setFillColor(sf::Color(255, 128, 0));
        text.setString(review.title);
        text.setPosition(startX, startY);
        window.draw(text);

        // رسم الريفيو
        text.setFillColor(sf::Color(0, 255, 0));
        text.setString(review.reviewTemplate);
        text.setPosition(startX, startY + (bounds.height * 0.07f));
        window.draw(text);

        // رسم الـ Input
        string displayStr = "> " + review.userInput + "_";
        text.setFillColor(sf::Color::White);
        text.setString(displayStr);
        text.setPosition(startX, pos.y + (bounds.height * 0.23f));
        window.draw(text);
    }
}
