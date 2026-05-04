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


    review.totalWords = 2;
    review.solutions[0] = "gateway";
    review.solutions[1] = "unknown";

    review.hints[0] = "A path or entrance to another place";
    review.hints[1] = "Something not identified or familiar";

    // Reset game state variables
    review.currentWordIdx = 0;
    review.userInput = "";
    review.errorMessage = "";
    review.isCleared = false;
}

void updateReviewInput(sf::Event& event, MovieReview& review) {
    if (review.isCleared) return;

    // Reset error state on any key press to resume typing
    if (review.errorMessage != "" && event.type == sf::Event::KeyPressed) {
        review.errorMessage = "";
        review.userInput = "";
        return;
    }

    // Process Character Entry
    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8) { // Handle Backspace
            if (!review.userInput.empty()) review.userInput.pop_back();
        }
        else if (event.text.unicode < 128 && event.text.unicode != 13) {
            review.userInput += static_cast<char>(event.text.unicode);
        }
    }

    // Process Submission on Enter
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        if (!review.userInput.empty()) {
            string temp = review.userInput;

            // Normalize input: Lowercase conversion and trimming
            for (auto & c: temp) c = tolower(c);
            temp.erase(0, temp.find_first_not_of(' '));
            temp.erase(temp.find_last_not_of(' ') + 1);

            // Validation against current array element
            if (temp == review.solutions[review.currentWordIdx]) {
                review.errorMessage = "";
                review.currentWordIdx++;
                review.userInput = "";

                // Check for overall win condition
                if (review.currentWordIdx >= review.totalWords) {
                    review.isCleared = true;
                }
            } else {
                // Trigger error state with contextual hint from the array
                review.errorMessage = "ACCESS DENIED\n\nHINT: " + review.hints[review.currentWordIdx];
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

    // Animation variables (Static to persist between frame calls)
    static float lineAlpha[5] = { 0, 0, 0, 0, 0 };
    static int currentLineVisible = 0;
    static int charCount = 0;
    static float charTimer = 0.0f;

    const float fadeSpeed = 165.0f;
    const float typeSpeed = 0.05f;

    // 1. Victory State Visuals: Fade-in and Typewriter animation
    if (review.isCleared) {
        string winLines[] = { "ACCESS GRANTED", "CONNECTION ESTABLISHED", "\"A GATEWAY TO THE UNKNOWN\"" };
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
                if (charTimer >= typeSpeed && charCount < (int)winLines[2].length()) {
                    charCount++;
                    charTimer = 0.0f;
                }
            }
        }

        for (int i = 0; i < totalWinLines; i++) {
            text.setCharacterSize(28);
            if (i < 2) {
                text.setFillColor(sf::Color(0, 255, 0, (sf::Uint8)lineAlpha[i]));
                text.setString(winLines[i]);
            } else if (i == 2 && currentLineVisible >= 2) {
                text.setFillColor(sf::Color(0, 255, 0));
                text.setString(winLines[2].substr(0, charCount));
            } else continue;

            sf::FloatRect tBounds = text.getLocalBounds();
            text.setOrigin(tBounds.width / 2.0f, tBounds.height / 2.0f);
            text.setPosition(pos.x, (pos.y - 70.f) + (i * 55.f));
            window.draw(text);
        }
        return;
    }

    // 2. Error State Visuals
    if (review.errorMessage != "") {
        string errLines[] = { "ACCESS DENIED", "HINT: " + review.hints[review.currentWordIdx], "[Press any key to retry]" };
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
    // 3. Main UI State
    else {
        // Reset animations when returning to normal state
        currentLineVisible = 0;
        charCount = 0;
        charTimer = 0.0f;
        for (int i = 0; i < 5; i++) lineAlpha[i] = 0.0f;

        float startX = pos.x - (bounds.width * 0.42f);
        float startY = pos.y - (bounds.height * 0.40f);

        // Render Movie Title
        text.setCharacterSize(20);
        text.setFillColor(sf::Color(255, 128, 0));
        text.setString(review.title);
        text.setPosition(startX + 40.f, startY + 20.f);
        window.draw(text);

        // Progress Tracker: Displays current word index vs total count
        string progress = to_string(review.currentWordIdx) + " / " + to_string(review.totalWords) + " WORDS";
        text.setCharacterSize(17);
        text.setFillColor(sf::Color(150, 150, 150));
        text.setString(progress);
        text.setPosition(startX + (bounds.width * 0.58f), startY + 25.f);
        window.draw(text);

        // Render Review Content
        text.setCharacterSize(17);
        text.setFillColor(sf::Color::Green);
        text.setString(review.reviewTemplate);
        text.setPosition(startX, startY + (bounds.height * 0.12f));
        window.draw(text);

        // Render User Input Field
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::White);
        text.setString("> " + review.userInput + "_");
        text.setPosition(startX, pos.y + (bounds.height * 0.23f));
        window.draw(text);
    }
}