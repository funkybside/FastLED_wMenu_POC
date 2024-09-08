#pragma once
#ifndef LCDMenu_h
#define LCDMenu_h

#include "Arduino.h"

#define MAIN_MENU_ITEM_COUNT 7
#define LED_PATTERN_COUNT 6

class LCDMenu : public LiquidCrystal {
  public:
    using LiquidCrystal::LiquidCrystal;

    // menu control functions  
    void initMenu(); // loads the hard-coded menu in default state
    void inputAdjust(bool isCW); // fire increment action where clockwise is positive
    void inputSelect(); // user command SW
    void drawMenu(); // refreshes the menu display
    
    // menu i/o functions
    void incrementHue(); // increments the hue value by 1
    void incrementPattern(bool isCW = true); // increments the currently selected pattern by 1
    bool getHueMode(); // returns the current hue auto mode
    bool getPatternMode(); // returns the current pattern auto mode
    uint8_t getCurHue(); // returns the current hue value
    uint8_t getCurFPS(); // returns the current fps value
    uint8_t getCurHueSpeed(); // returns the current hue change speed
    uint8_t getCurPatternNum(); // returns the currently selected pattern name

  private:
    uint8_t _curMenuItem = 0; // idx of current active menu item
    uint8_t _curPattern = 0; // idx of current active pattern
    uint8_t _curHue = 0; // value of current base hue
    uint8_t _curHueSpeed = 20; // millisconds per hue++ increment when in auto mode 
    uint8_t _curFPS = 120; // frames per second implemneted as FastLED delay per loop
    
    bool _navTop = true; // if true, input is line1, else line 2
    bool _hueModeAuto = true; //auto / manual mode; 
    bool _patternModeAuto = true; // if true, cycles patterns automatically. 
    
    const String menuItems[MAIN_MENU_ITEM_COUNT] = {"Pattern Mode", "Select Pattern", "Set Hue Mode", "Set Hue Speed", "Set Hue Color", "Set FPS", "Reset Defaults"};
    const String patternList[LED_PATTERN_COUNT] = {"Rainbow", "Glitterbow", "Confetti", "Sinelon", "Juggle", "BPM"};

    String _line1 = "Menu L1";
    String _line2 = "Menu L2";
};

#endif


