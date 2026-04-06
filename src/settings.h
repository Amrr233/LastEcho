#ifndef LASTECHO_SETTINGS_H
#define LASTECHO_SETTINGS_H

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Data.h" // ضروري عشان الـ AppState والـ States

struct settingsMenu {
 sf::Texture labelTex[3];
 sf::Sprite labelSprites[3];
 // 1. اللوحة الخلفية (البوردة الخشب)
 sf::Sprite settings_window;
 sf::Texture bgTex;
 // bg behind the settings window
 sf::Sprite backSprite; // صورة الكليه اللى ورا
 sf::Texture backTex;

 // 2. التكستشرز اللي هنستخدمها لكل السلايدرز
 sf::Texture volbar_tex;    // صورة الخط الخشبي (المسار)
 sf::Texture sliderTexture; // صورة المقبض (الـ Knob)

 // --- السلايدرز التلاتة ---

 // Master Slider (الصوت الكلي)
 sf::Sprite masterBar;
 sf::Sprite masterHandle;
 float masterVolume = 50.0f;

 // Music Slider (المزيكا)
 sf::Sprite musicBar;
 sf::Sprite musicHandle;
 float musicVolume = 50.0f;

 // SFX Slider (المؤثرات)
 sf::Sprite sfxBar;
 sf::Sprite sfxHandle;
 float sfxVolume = 50.0f;


 // escape
  sf::Sprite backButton;
  sf::Texture backBtnTex;

  // هنضيف دي عشان نعرف الماوس فوق الزرار ولا لأ (للـ Hover effect)
  bool isBackHovered = false;

 // --- الدوال الأساسية للمنيو ---
 void init(float W, float H);
 void updateMaster(float delta);
 void updateMusic(float delta);
 void updateSFX(float delta);

 // دالة الرسم (اللي بنناديها في المين)
 void draw(sf::RenderWindow &window);
};

// إعلان الكائن عشان يقرأه أي ملف تاني (زي المين)
extern settingsMenu settings;

// دالة تحديث المنطق والتحكم (الماوس والكيبورد)
void SettingsUpdate(sf::RenderWindow& window, AppState& currentState);

#endif // LASTECHO_SETTINGS_H