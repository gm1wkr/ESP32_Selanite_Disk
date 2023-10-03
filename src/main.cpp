/**
 * ----------------------------------------------------------------------------|
 * PHYSICAL PROJECT - Selanite Nite Light                                      |
 * ----------------------------------------------------------------------------|
 * Target:      Espressif ESP32 Dev Kit C
 *
 * GPIO 12 In   SP momentary switch to change displayed effect.
 * GPIO 14 Out  5px WS2812B strip as an uplight for crystal.  Currently powered
 *              from VBUS. 
 *
 * ----------------------------------------------------------------------------|
 * History
 * 03 Oct 2023  wkr     Project Start.  Scaffolding.
 * ----------------------------------------------------------------------------|
*/

#include<Arduino.h>
#define FASTLED_INTERNAL
#include<FastLED.h>
#include <OneButton.h>

#define COLOR_ORDER    GRB
#define BRIGHTNESS     160
#define LED_PIN         14
#define NUM_LEDS         5

#define PATTERN_BUTTON_PIN  12
#define NUM_PATTERNS         2


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

DEFINE_GRADIENT_PALETTE(pFire){
    0, 230, 60, 0,
    16, 255, 0, 0,
    48, 255, 70, 0,
    76, 255, 180, 0,
    128, 255, 60, 0,
    196, 122, 0, 0,
    220, 186, 64 , 0,
    255, 255, 20, 0,
};

void nextPattern();
void nightLightCool();
void nightLightFire();

void setup() {
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    btn.attachClick(nextPattern);
}

void loop() {
    switch(currentPattern)
    {
        case 0:
            nightLightCool();
            break;

        case 1:
            nightLightFire();
            break;
    }

    FastLED.show();

    btn.tick();
}

void nextPattern() {
  currentPattern = (currentPattern + 1) % NUM_PATTERNS;
}

void nightLightCool()
{
    CRGBPalette16 ice = pBlueIce;
    uint8_t brightness = beatsin16(2, 92, 128, 0, 0);

    for(uint8_t i = NUM_LEDS - 1; i < 0; i--){
        leds[i] = ColorFromPalette(ice, colourIndex[i]);
    }

    fill_palette(leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, ice, brightness, LINEARBLEND);

    EVERY_N_MILLISECONDS(200) {
        paletteIndex++;
    }
}

void nightLightFire()
{
    CRGBPalette16 fire = pFire;
    uint8_t brightness = beatsin8(1, 64, 92, 0, 0);

    for (uint8_t i = NUM_LEDS -1; i < 0; i--) {
        leds[i] = ColorFromPalette(fire, colourIndex[i]);
    }

    fill_palette(leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, fire, brightness, LINEARBLEND);

    EVERY_N_MILLISECONDS(120) {
        paletteIndex++;
    }
}
