
#include <LiquidCrystal.h>
#include <FastLED.h>
#include "LCDMenu.h"

FASTLED_USING_NAMESPACE

// Declarations for LCD Display
#define LCD_WIDTH 16
#define LCD_HEIGHT 2
#define LCD_RS 8
#define LCD_ENABLE 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

// Create the LCDMenu object, setting the reset, enable, and 4 data lines
LCDMenu LCD(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Declariations for Rotary Encoder 
#define ROT_CLK 19
#define ROT_DT 18
#define ROT_SW 20
#define ROT_DEBOUNCE_MS 50

int counter = 0;
int currentStateCLK;
int lastStateCLK;
unsigned long lastButtonPress = 0;

// Declarations for Fast LED elements
#define LED_DATA_PIN 3
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 32
#define DEFAULT_BRIGHTNESS 32
#define MAX_BRIGHTNESS 128
#define PATTERN_CHANGE_SECONDS 10
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0])) // used to advance pattern in auto mode

unsigned long lastHueUpdate = 0;  // used in place of EVERY_N_MILLISECONDS for hue update, since variable timing
unsigned long lastPatternUpdate = 0;  // used in place of EVERY_N_SECONDS for pattern update, since variable timing
unsigned long lastLCDRefresh = 0; // used to set a forced minimum LCD menue redraw

CRGB leds[NUM_LEDS];

void setup() {
  // Initialize the LCD Display & Menu
  LCD.begin(LCD_WIDTH, LCD_HEIGHT);
  LCD.initMenu();

  // set encoder input pins
  pinMode(ROT_CLK, INPUT);
  pinMode(ROT_DT, INPUT);
  pinMode(ROT_SW, INPUT_PULLUP);

  // get initial state of rotary CLK pin
  lastStateCLK = digitalRead(ROT_CLK);

  delay(2000); // 2.5 second delay for recovery

  // initialize LED Strip
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(DEFAULT_BRIGHTNESS); // set master brightness
}

void rainbow() {
  // FastLED's built in rainbow generator
  // *** Last param is delta hue, maybe look in to making adjustable 
  fill_rainbow(leds, NUM_LEDS, LCD.getCurHue(), 7);
}

void addGlitter(fract8 chanceOfGlitter) {
  // *** chanceOfGlitter could be adjustable
  if (random8() < chanceOfGlitter) {
    leds[random16(NUM_LEDS)] += CRGB::White;
  }
}

void rainbowWithGlitter() {
  // built-in FastLED rainbow, plus sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti() {
  // random colored spekcles that blink in and fade smoothly
  fadeToBlackBy(leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV(LCD.getCurHue() + random8(64), 200, 255);
}

void sinelon() {
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS-1); 
  leds[pos] += CHSV(LCD.getCurHue(), 255, 192);
}

void bpm() {
  // colored stripes pulsing at the defined Beats-Per-Minute
  uint8_t BeatsPerMinute = 62;
  uint8_t curHue = 0;
  curHue = LCD.getCurHue();
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, curHue+(i*2), beat-curHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy(leds, NUM_LEDS, 20);
  uint8_t dothue = 0;
  for(int i = 0; i < 8; i++) {
    leds[beatsin16(i+7, 0, NUM_LEDS-1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

// list of patterns that can be cycled through, each defined as a separate function
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm};

void subloop_rotary() {
  // subset of loop that handles rotary encoder events

  // read the current state of CLK
  currentStateCLK = digitalRead(ROT_CLK);

  // if the last and current state of CLK are different, then pulse occured
  if (currentStateCLK != lastStateCLK) {
    // if the DT state is same as CLK state, then
    // the rotation was CCW
    LCD.inputAdjust(digitalRead(ROT_DT) == currentStateCLK);
    LCD.drawMenu();
  }

  // remember last CLK state
  lastStateCLK = currentStateCLK;

  // check the button state, if LOW, is pressed
  if (digitalRead(ROT_SW) == LOW) {
    // simple debounce handle - if ROT_DEBOUNCE_MS have passed since last LOW pulse, 
    // it means we'll handle the button press
    if (millis() - lastButtonPress > ROT_DEBOUNCE_MS) {
      LCD.inputSelect();
      LCD.drawMenu();
    }
    // remember the last button press time
    lastButtonPress = millis();
  }
  // add as mall delay to help debounce readings
  delay(1);
}

void subloop_leds() {
  // subset of loop for LED array control

  // call the curernt pattern once, upadting the led array
  gPatterns[LCD.getCurPatternNum()]();

  // send data to the actual strip
  FastLED.show();

  // insert a delay to control framerate
  FastLED.delay(1000 / LCD.getCurFPS());
  
  // update the hue color if:
  //  -set to auto mode, and
  //  - sufficient time has ellapsed
  if (LCD.getHueMode() && (millis() - lastHueUpdate > LCD.getCurHueSpeed())) {
    LCD.incrementHue();
    lastHueUpdate = millis();
  }

  // update the pattern if:
  //  -set to auto mode, and
  //  -sufficent time has ellapsed
  if (LCD.getPatternMode() && ((millis() - lastPatternUpdate) / 1000 > PATTERN_CHANGE_SECONDS)) {
    LCD.incrementPattern();
    lastPatternUpdate = millis();
  }
}

void loop() {
  // main loop code
  subloop_rotary();
  subloop_leds();
}
