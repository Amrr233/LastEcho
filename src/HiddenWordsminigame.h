#ifndef HIDDENWORDS_H
#define HIDDENWORDS_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

// Maximum capacity for the word system to ensure memory safety
const int MAX_WORDS = 5;

struct MovieReview {
    string title;            // Movie title header
    string reviewTemplate;   // Main text with underscores

    string solutions[MAX_WORDS];
    string hints[MAX_WORDS];
    int totalWords = 0;      // Actual count of words in the current level

    int currentWordIdx = 0;  // Tracks progress through word array
    string userInput = "";   // Buffer for active keyboard input
    bool isCleared = false;  // Level completion flag
    string errorMessage = ""; // Buffer for error/hint messages
};

// Function prototypes for game lifecycle management
void initReviewGame(MovieReview& review);
void updateReviewInput(sf::Event& event, MovieReview& review);
void drawReviewGame(sf::RenderWindow& window, sf::Sprite& screenBg, sf::Font& font, MovieReview& review);

#endif