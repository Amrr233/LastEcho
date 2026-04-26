#include "HiddenWordsminigame.h"
#include <iostream>

void initReviewGame(MovieReview& review) {
    review.title = "     The Hidden Secret (2013)";
    review.reviewTemplate =
        "This doesn't feel like a story anymore, it feels like something breaking through\n"
        "reality itself.\n\n"
        "Every scene pulls you deeper, as if the world is forming a ____ between what\n"
        "is real and what is not.\n\n"
        "At some point, meaning collapses completely, and everything starts pointing\n"
        "toward what lies beyond.\n\n"
        "And what's ______ starts getting revealed through every passing moment,\n"
        "as if it was always there waiting for its time to shine.\n";

    review.solutions.clear();
    review.solutions.push_back("gateway");
    review.solutions.push_back("unknown");

    review.hints.clear();
    review.hints.push_back("A path or entrance to another place");
    review.hints.push_back("Something not identified or familiar");

    review.currentWordIdx = 0;
    review.userInput = "";
    review.errorMessage = "";
    review.isCleared = false;
}

void updateReviewInput(sf::Event& event, MovieReview& review) {
    if (review.isCleared) return;

    // لو فيه رسالة خطأ، أي ضغطة زرار تمسحها وترجعنا لشاشة الكتابة
    if (review.errorMessage != "" && event.type == sf::Event::KeyPressed) {
        review.errorMessage = "";
        review.userInput = ""; // بنصفر الـ input عشان يبدأ على نظافة
        return;
    }

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) { // Backspace
            if (!review.userInput.empty()) review.userInput.pop_back();
        }
        else if (event.text.unicode < 128 && event.text.unicode != 13) {
            review.userInput += static_cast<char>(event.text.unicode);
        }
    }

if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
    if (!review.userInput.empty()) {
        string temp = review.userInput;
        // 1. تنظيف الكلمة تماماً
        for (auto & c: temp) c = tolower(c);
        temp.erase(0, temp.find_first_not_of(' '));
        temp.erase(temp.find_last_not_of(' ') + 1);

        // 2. المقارنة
        if (temp == review.solutions[review.currentWordIdx]) {
            // لو صح: بنصفر الخطأ ونزود الـ Index للكلمة اللي بعدها
            review.errorMessage = "";
            review.currentWordIdx++;
            review.userInput = "";

            if (review.currentWordIdx >= review.solutions.size()) {
                review.isCleared = true;
            }
        } else {
            // لو غلط: بنعرض الهينت الخاص بالكلمة الحالية (اللي اللاعب واقف عندها)
            // بما إن currentWordIdx لسه م زادش، هيفضل واقف عند 0 لو بيغلط في أول كلمة
            review.errorMessage = "ACCESS DENIED\n\nSYSTEM LOCKED\n\nHINT: " + review.hints[review.currentWordIdx] + "\n\n[Press any key to retry]";
            // ملحوظة: م بنصفرش الـ userInput هنا عشان لو اللاعب عايز يشوف هو كتب إيه غلط قبل ما الشاشة تقلب
        }
    }
}
}

void drawReviewGame(sf::RenderWindow& window, sf::Sprite& screenBg, sf::Font& font, MovieReview& review) {
    window.draw(screenBg);

    sf::Vector2f pos = screenBg.getPosition();
    sf::FloatRect bounds = screenBg.getGlobalBounds();
    sf::Text text;
    text.setFont(font);

    // --- متغيرات الأنيميشن ---
    static float lineAlpha[5] = { 0, 0, 0, 0, 0 };
    static int currentLineVisible = 0;
    static int charCount = 0;
    static float charTimer = 0.0f;

    const float fadeSpeed = 165.0f;
    const float typeSpeed = 0.05f;

    // 1. شاشة الفوز (Fade + Typewriter للسطر الأخير)
    if (review.isCleared) {
        string winLines[] = {
            "ACCESS GRANTED",
            "CONNECTION ESTABLISHED",
            "\"A GATEWAY TO THE UNKNOWN\""
        };
        int totalWinLines = 3;

        if (currentLineVisible < totalWinLines) {
            if (currentLineVisible < 2) {
                lineAlpha[currentLineVisible] += fadeSpeed * 0.016f;
                if (lineAlpha[currentLineVisible] >= 255.0f) {
                    lineAlpha[currentLineVisible] = 255.0f;
                    currentLineVisible++;
                }
            } else {
                charTimer += 0.016f;
                if (charTimer >= typeSpeed && charCount < winLines[2].length()) {
                    charCount++;
                    charTimer = 0.0f;
                }
            }
        }

        for (int i = 0; i < totalWinLines; i++) {
            text.setCharacterSize(28);
            text.setFillColor(sf::Color(0, 255, 0));

            if (i < 2) {
                text.setFillColor(sf::Color(0, 255, 0, (sf::Uint8)lineAlpha[i]));
                text.setString(winLines[i]);
            } else if (i == 2 && currentLineVisible >= 2) {
                text.setString(winLines[2].substr(0, charCount));
            } else continue;

            sf::FloatRect tBounds = text.getLocalBounds();
            text.setOrigin(tBounds.width / 2.0f, tBounds.height / 2.0f);
            text.setPosition(pos.x, (pos.y - 70.f) + (i * 55.f));
            window.draw(text);
        }
        return;
    }

    // 2. شاشة الخطأ
    if (review.errorMessage != "") {
        string errLines[] = {
            "ACCESS DENIED",
            "HINT: " + review.hints[review.currentWordIdx],
            "[Press any key to retry]"
        };
        int totalErrLines = 3;

        if (currentLineVisible < totalErrLines) {
            lineAlpha[currentLineVisible] += fadeSpeed * 0.016f;
            if (lineAlpha[currentLineVisible] >= 255.0f) {
                lineAlpha[currentLineVisible] = 255.0f;
                currentLineVisible++;
            }
        }

        for (int i = 0; i < totalErrLines; i++) {
            if (i <= currentLineVisible) {
                text.setCharacterSize(28);
                text.setFillColor(sf::Color(255, 0, 0, (sf::Uint8)lineAlpha[i]));
                text.setString(errLines[i]);

                sf::FloatRect tBounds = text.getLocalBounds();
                text.setOrigin(tBounds.width / 2.0f, tBounds.height / 2.0f);
                text.setPosition(pos.x, (pos.y - 60.f) + (i * 45.f));
                window.draw(text);
            }
        }
    }
    // 3. شاشة اللعب العادية
    else {
        currentLineVisible = 0;
        charCount = 0;
        charTimer = 0.0f;
        for (int i = 0; i < 5; i++) lineAlpha[i] = 0.0f;

        float startX = pos.x - (bounds.width * 0.42f);
        float startY = pos.y - (bounds.height * 0.40f);

        // --- العنوان ---
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(255, 128, 0));
        text.setString(review.title);
        text.setPosition(startX + 40.f, startY + 20.f);
        window.draw(text);

        // --- عداد الكلمات (Progress Tracker) ---
        // يظهر في ركن العنوان بلون هادئ
        std::string progress = std::to_string(review.currentWordIdx) + " / " + std::to_string(review.solutions.size()) + " WORDS";
        text.setCharacterSize(17);
        text.setFillColor(sf::Color(150, 150, 150));
        text.setString(progress);
        text.setPosition(startX + (bounds.width * 0.58f), startY + 25.f);
        window.draw(text);

        // --- الريفيو ---
        text.setCharacterSize(17);
        text.setFillColor(sf::Color::Green);
        text.setString(review.reviewTemplate);
        text.setPosition(startX, startY + (bounds.height * 0.12f));
        window.draw(text);

        // --- الإدخال ---
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setString("> " + review.userInput + "_");
        text.setPosition(startX, pos.y + (bounds.height * 0.23f));
        window.draw(text);
    }
}