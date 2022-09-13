/**
 * Arduino code for portable RGB Lamp
 * This code is used for an arduino pro mini connected to a battery, a switch for turning on and off and a push button.
 * Has 2 working modes:
 * 1º Like now, save in eeprom the current mode after every switch,
 *  this consumes more eeprom savings (those are limited) but is easier to use
 * 2º With long click you save the current mode in the eeprom data
 * 
 * The lamp work as follow:
 * 2 modes
 * - White
 * - Colors
 * 
 *  Single click: Change brightness
 *  Double click: if white change warm, if colors change pattern
 *  Long click: Switch between white <-> colors
 * 
 * To upload the code using an arduino nano to connect to pc, you have to push reset button
 * in the pro mini when is already to upload
 * 
 * At first run you have to uncomment the saveToEEPROM() call in the setup(), then you have to comment that, so you'll have
 * to upload 2 times the code
 *
 */

#include <Arduino.h>
#include <FastLED.h>
#include <PinButton.h>
#include <EEPROM.h>


#define LED_PIN 9
#define BUTTON_PIN 2
#define NUM_LEDS 16

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
struct CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100
int brightness = 255; //maximum brightness


#include "solid_color_mode.h"
#include "palette_mode.h"
#include "effect_mode.h"

PinButton FunctionButton(BUTTON_PIN);
int setMode = 0;
bool white = false;
bool flag = false;

void saveInEEPROM(){
  EEPROM.update(0, setMode);        
  EEPROM.update(1, colorCounter);
  EEPROM.update(2, paletteCounter);
  EEPROM.update(3, gCurrentPatternNumber);  
}

void doubleClick(){ //if white change warm, if colors change pattern
  if (white){
    flag = !flag;
  } else {
    // to-do: An array who pick random effect/palette
    paletteCounter++;
    if (paletteCounter > 11) {paletteCounter = 0;}
  }
}

void setup() {

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(brightness);
  FastLED.clear();

  // at first run uncomment this for initializing the eeprom data
  //saveInEEPROM();

  setMode = EEPROM.read(0);
  colorCounter = EEPROM.read(1);
  paletteCounter = EEPROM.read(2);
  gCurrentPatternNumber = EEPROM.read(3);
}

void loop() {
  FunctionButton.update();

  if (FunctionButton.isSingleClick()) //brightness
    FastLED.setBrightness(FastLED.getBrightness() + 50 );
    if (FastLED.getBrightness() > 250) FastLED.setBrightness(50);

  if (FunctionButton.isDoubleClick()) { //Modes
    doubleClick();
  }
  else if (FunctionButton.isLongClick()) { //White - Color
    white = !white;
    saveInEEPROM();
  }
  
  if (white){
    if (flag)
      fill_solid( leds, NUM_LEDS, CHSV(0, 0, 192)); // White
     else 
      fill_solid(leds, NUM_LEDS, CHSV(44,13,99)); //Warm white ?
  } else {
    //FastLED.setBrightness(brightness);
    ChangePalettePeriodically();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    FillLEDsFromPaletteColors(startIndex);
  }


  // switch (setMode)
  // {
  // case 0: // White
  //   fill_solid( leds, NUM_LEDS, CHSV(0, 0, 192)); // White
  //   fill_solid(leds, NUM_LEDS, CHSV(44,13,99)); //Warm white ?
  //   break;
  // case 1: // Solid Color
  //   if (colorCounter % 2 == 0) {
  //     float breath = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
  //     FastLED.setBrightness(breath);
  //   }
  //   else FastLED.setBrightness(BRIGHTNESS);
  //   ChangeColorPeriodically();
  //   break;
  // case 2: //Palettes
  //   FastLED.setBrightness(BRIGHTNESS);
  //   ChangePalettePeriodically();
  //   static uint8_t startIndex = 0;
  //   startIndex = startIndex + 1;
  //   FillLEDsFromPaletteColors(startIndex);
  //   break;
  // case 3: //Effects
  //   gPatterns[gCurrentPatternNumber]();
  //   break;
  // }


  FastLED.show();
  FastLED.delay(2000 / UPDATES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}