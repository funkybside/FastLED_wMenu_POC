#include <Arduino.h>
#include <LiquidCrystal.h>

#include "LCDMenu.h"

void LCDMenu::initMenu() {

  drawMenu();
}

void LCDMenu::drawMenu() {
  char buffer [4];

  // build menu line 1 text
  _line1 = menuItems[_curMenuItem];

  // build menu line 2 text:
  switch (_curMenuItem) {
    case 0: // "Pattern Mode"
      _line2 = (_patternModeAuto) ? "AUTO" : "MANUAL";
      break;
    case 1: // "Select Pattern"
      _line2 = patternList[_curPattern];
      break;
    case 2: // "Set Hue Mode"
      _line2 = (_hueModeAuto) ? "AUTO" : "MANUAL";
      break;
    case 3: // "Set Hue Speed"
      _line2 = "ms/Hue: "; 
      _line2 += itoa(_curHueSpeed, buffer, 10);
      break;
    case 4: // "Set Hue Color"
      _line2 = "Hue Val: "; 
      _line2 += itoa(_curHue, buffer, 10);
      break;
    case 5: // "Set FPS"
      _line2 = "Val: ";
      _line2 += itoa(_curFPS, buffer, 10);
      break;
    case 6: // "Reset Defaults"
      _line2 = "<--> to confirm";
      break;
    default:
      _line2 = "{no curItem}";
  }

   // clears & redraws the screen
  clear();
  setCursor(0,0);
  print(_line1);
  setCursor(0,1);
  print(_line2);
 
  //set cursor blink to show which line input is active
  if (_navTop) {
    setCursor(15,0);
  } else {
    setCursor(15,1);
  }
  blink();

  delay(1);
}

void LCDMenu::inputSelect() {
  // trigger input select action e.g., when enter button is pressed
  // current action -> toggle line select
  _navTop = !_navTop;
}

void LCDMenu::inputAdjust(bool isCW) {
  // trigger +/- change in current line select
  // if line select is top, change menu
  // if line select is bottom, adjust setting

  if (_navTop) {
    // we either need to add 1 or subtract one to the menu selected, caring to wrap if needed
    if (isCW) { 
      // command is CW / positive 
      if (_curMenuItem < (MAIN_MENU_ITEM_COUNT-1)) {
        // we have room to increment
        _curMenuItem += 1;
      } else {
        // need to wrap around to start
        _curMenuItem = 0;
      }    
    } else {
      // command is CCW / negative
      if (_curMenuItem > 0) {
        // we have room to decrement
        _curMenuItem -= 1;
      } else {
        // need to wrap around to end
        _curMenuItem = (MAIN_MENU_ITEM_COUNT-1);
      }
    }
  } else {
    // handle if cursor is on 2nd line  
    switch (_curMenuItem) {
      case 0: // "Pattern Mode"
        _patternModeAuto = !_patternModeAuto;
        break;
      case 1: // "Select Pattern"
        // increment or decrement current pattern
        // if it's in auto, switch to manual after stepping pattern
        this->incrementPattern(isCW);
        _patternModeAuto = false;
        break;
      case 2: // "Hue Mode"
        _hueModeAuto = !_hueModeAuto;
        break;
      case 3: // "Hue Speed"
        // increment or decrement hue speed, min 1, max 255
        if (isCW) {
          if (_curHueSpeed != 255) {
            _curHueSpeed += 1;
          } else { // we need to skip zero
            _curHueSpeed += 2;
          }
        } else {
          if (_curHueSpeed != 1) {
            _curHueSpeed -= 1;
          } else { // we need to skip zero
            _curHueSpeed -= 2;
          }
        }
        break;
      case 4: // "Hue Color"
        // hue value is active setting
        // set manual hue mode if was auto
        _curHue = (isCW) ? (_curHue + 1) : (_curHue - 1);
        _hueModeAuto = false;
        break;
      case 5: // "FPS"
        // increment or decrement fps, min 1, max 255
        if (isCW) {
          if (_curFPS != 255) {
            _curFPS += 1;
          } else { // we need to skip zero
            _curFPS += 2;
          }
        } else {
          if (_curFPS != 1) {
            _curFPS -= 1;
          } else { // we need to skip zero
            _curFPS -= 2;
          }
        }
        break;
      case 6: // "Reset Defaults"
        // set default menu/mode, pattern, hue mode, hue speed, and fps
        _patternModeAuto = true;
        _navTop = true;
        _curMenuItem = 0;
        _curPattern = 0;
        _hueModeAuto = true;
        _curHue = 0;
        _curHueSpeed = 20;
        _curFPS = 120;
        break;
      default:
        _line2 = "case not def";
    }
  }
}

void LCDMenu::incrementHue() {
  // increments the hue value by the set speed
  _curHue ++;
} 

void LCDMenu::incrementPattern(bool isCW) {
  // increments (default) or decrements the pattern
  if (isCW) {
    if (_curPattern != (LED_PATTERN_COUNT-1)) {
      _curPattern += 1;
    } else { // we need to skip zero
      _curPattern = 0;
    }
  } else {
    if (_curPattern != 0) {
      _curPattern -= 1;
    } else { // we need to skip zero
      _curPattern = (LED_PATTERN_COUNT-1);
    }
  }
}

bool LCDMenu::getHueMode() {
  // returns the current hue auto mode
  return _hueModeAuto;
} 

bool LCDMenu::getPatternMode() {
  // returns the current hue auto mode
  return _patternModeAuto;
} 

uint8_t LCDMenu::getCurHue() {
  // returns the current hue value
  return _curHue;
}

uint8_t LCDMenu::getCurFPS() {
  // returns the current fps value
  return _curFPS;
}

uint8_t LCDMenu::getCurHueSpeed() {
  // returns the current hue change speed
  return _curHueSpeed;
}

uint8_t LCDMenu::getCurPatternNum() {
  // returns the currently selected pattern name
  return _curPattern;
}