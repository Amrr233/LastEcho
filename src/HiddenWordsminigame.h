#ifndef HIDDENWORDS_H
#define HIDDENWORDS_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

using namespace std;

// ده الـ Struct اللي هيشيل بيانات المرحلة
struct MovieReview {
    string title;            // اسم الفيلم
    string reviewTemplate;   // النص بالـ ____
    vector<string> solutions; // الكلمات الصح
    vector<string> hints;     // الهينت لكل كلمة
    int currentWordIdx = 0;   // واصلين للكلمة الكام
    string userInput = "";    // اللي اللاعب بيكتبه حالياً
    bool isCleared = false;   // هل خلص المرحلة دي؟
};

// الفانكشنز اللي هنستخدمها في الـ cpp
void initReviewGame(MovieReview& review);
void updateReviewInput(sf::Event& event, MovieReview& review);
void drawReviewGame(sf::RenderWindow& window, sf::Sprite& screenBg, sf::Font& font, MovieReview& review);

#endif