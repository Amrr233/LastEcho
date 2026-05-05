#ifndef HIDDENWORDS_H
#define HIDDENWORDS_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

const int MAX_WORDS = 5;

struct MovieReview {
    string title;
    string reviewTemplate;

    string solutions[MAX_WORDS];
    string hints[MAX_WORDS];
    int totalWords = 0;

    int currentWordIdx = 0;
    string userInput = "";
    bool isCleared = false;
    string errorMessage = "";
};


void initReviewGame(MovieReview& review);
void updateReviewInput(sf::Event& event, MovieReview& review);
void drawReviewGame(sf::RenderWindow& window, sf::Sprite& screenBg, sf::Font& font, MovieReview& review);

#endif