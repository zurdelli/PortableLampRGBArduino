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
 *  Double click:
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
#define BRIGHTNESS 255 //maximum brightness
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
struct CRGB leds[NUM_LEDS];
#define UPDATES_PER_SECOND 100

#include "solid_color_mode.h"
#include "palette_mode.h"
#include "effect_mode.h"

PinButton FunctionButton(BUTTON_PIN);
int setMode = 0;
bool white = false;

void saveInEEPROM(){
  EEPROM.update(0, setMode);        
  EEPROM.update(1, colorCounter);
  EEPROM.update(2, paletteCounter);
  EEPROM.update(3, gCurrentPatternNumber);  
}

void singleClick(){
    switch (setMode)
    {
    case 0:
      FastLED.setBrightness(FastLED.getBrightness() + 30 );
      if (FastLED.getBrightness() > 250) FastLED.setBrightness(100);
      break;
    case 1:
      colorCounter++;
      if (colorCounter > 17) {colorCounter = 0;}
      break;
    case 2:
      paletteCounter++;
      if (paletteCounter > 11) {paletteCounter = 0;}
      break;
    case 3:
      nextPattern();     
      break;
     }
}

void setup() {

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
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

  if (FunctionButton.isSingleClick())
    singleClick();
    // white = !white;
  
    // 

  if (FunctionButton.isDoubleClick()) {
    setMode++;
    if (setMode > 3) {setMode = 0;}
  }
  else if (FunctionButton.isLongClick()) {
  saveInEEPROM();
  }
  
  switch (setMode)
  {
  case 0: // White
    fill_solid( leds, NUM_LEDS, CHSV(0, 0, 192));
    break;
  case 1: // Solid Color
    if (colorCounter % 2 == 0) {
      float breath = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
      FastLED.setBrightness(breath);
    }
    else FastLED.setBrightness(BRIGHTNESS);
    ChangeColorPeriodically();
    break;
  case 2: //Palettes
    FastLED.setBrightness(BRIGHTNESS);
    ChangePalettePeriodically();
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    FillLEDsFromPaletteColors(startIndex);
    break;
  case 3: //Effects
    gPatterns[gCurrentPatternNumber]();
    break;
  }

  // if (white){

  // } else {
    
  // }

  FastLED.show();
  FastLED.delay(2000 / UPDATES_PER_SECOND);
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}