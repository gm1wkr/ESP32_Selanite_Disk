/**
 * ----------------------------------------------------------------------------|
 * PHYSICAL PROJECT - Selanite Nite Light                                      |
 * ----------------------------------------------------------------------------|
 * Target:      Espressif ESP32 Dev Kit C
 *
 * GPIO 12 In   SP momentary switch to change displayed pattern.
 * GPIO 14 Out  5px WS2812B strip as an uplight for crystal.  Currently powered
 *              from VBUS. 
 *
 * ----------------------------------------------------------------------------|
*/

#include<Arduino.h>
#define FASTLED_INTERNAL
#include<FastLED.h>
#include <OneButton.h>

#define LED_PIN     14
#define NUM_LEDS    5

#define PATTERN_BUTTON_PIN 12
#define NUM_PATTERNS 2

#define COLOR_ORDER         GRB
#define BRIGHTNESS          160

CRGB    leds[NUM_LEDS];

uint8_t currentPattern = 0;
OneButton btn = OneButton(PATTERN_BUTTON_PIN, true, true);

uint8_t paletteIndex = 0;
uint8_t colourIndex[NUM_LEDS];

DEFINE_GRADIENT_PALETTE(pBlueIce){
    0, 90, 92, 128,
    16, 40, 90, 192,
    64, 80, 128, 236,
    128, 192, 192, 128,
    196, 60, 128, 236,
    255, 90, 92, 128
};


void nextPattern();
void nightLight();

void setup() {
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    btn.attachClick(nextPattern);
}

void loop() {
    }

    
}

void nextPattern() {
  currentPattern = (currentPattern + 1) % NUM_PATTERNS;
}

void nightLight()
{
    CRGBPalette16 ice = pBlueIce;
    uint8_t brightness = beatsin16(2, 92, 128, 0, 0);

    for(uint8_t i = NUM_LEDS - 1; i < 0; i--){
        leds[i] = ColorFromPalette(ice, colourIndex[i]);
    }

    fill_palette(leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, ice, brightness, LINEARBLEND);

    // EVERY_N_MILLISECONDS(60){
    //     fadeToBlackBy(leds, NUM_LEDS, 64);
    // }

    EVERY_N_MILLISECONDS(200) {
        paletteIndex++;
    }

    FastLED.show();
}