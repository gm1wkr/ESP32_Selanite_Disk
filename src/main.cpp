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
 * 03 Oct 2023  wkr     - Project Start.  Scaffolding.
 * 03 Oct 2023  wkr     - Library OneButton Added.
 *                      - Manages hardware buttons and switches.
 * 
 * 07 Oct 2023  wkr     - Add HW momentary switch
 *                      - Add Effect changer functionality.
 *                      - Add Crossfade on effect change functionality.
 *                      - Add effects: fire, embers, coals and
 *                        juicyPlum.
 *                      - Rename symbols (compiled, not tested on HW)
 * 08 Oct 2023  wkr     - Rename Symbols (Tests passed)
 * 
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
#define NUM_PATTERNS         8


CRGB    bufSource1[NUM_LEDS];
CRGB    bufSource2[NUM_LEDS];
CRGB    bufOut[NUM_LEDS];

uint8_t blendAmount         = 0;
uint8_t sourceEffect1       = 0;
uint8_t sourceEffect2       = 1;
bool    useSourceEffect1    = false;

uint8_t currentEffect = 0;
OneButton btn = OneButton(PATTERN_BUTTON_PIN, true, true);

uint8_t paletteIndex = 0;
uint8_t colourIndex[NUM_LEDS];

CRGBPalette16 firePalette = HeatColors_p;

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
    48, 255, 70, 96,
    76, 255, 180, 0,
    128, 255, 60, 96,
    196, 122, 0, 0,
    220, 186, 64 , 0,
    255, 255, 20, 0,
};

DEFINE_GRADIENT_PALETTE(pPlum){
    0, 209, 0, 209,
    48, 255, 16, 209,
    64, 192, 32, 255,
    128, 255, 16, 209,
    192, 255, 0, 230,
    255, 209, 0, 209,
};

DEFINE_GRADIENT_PALETTE(pEmerald){
    0, 20, 200, 0,
    64, 40, 255, 10,
    128, 40, 255, 0,
    192, 70, 200, 20,
    255, 0, 255, 20,
};

DEFINE_GRADIENT_PALETTE(pMoon){
    0, 210, 255, 245,
    48, 168, 168, 165,
    128, 98, 120, 120,
    192, 159, 168, 179,
    255, 220, 220, 245,
};

void nextEffect();
void runEffect(uint8_t pattern, CRGB *LEDBuffer);
void nightLightCool(CRGB *LEDBuffer);
void nightLightWarm(CRGB *LEDBuffer);
void fireEffect(CRGB *LEDBuffer);
void embersEffect(CRGB *LEDBuffer);
void coalsEffect(CRGB *LEDBuffer);
void juicyPlumEffect(CRGB *LEDBuffer);
void emeraldEffect(CRGB *LEDBuffer);
void coolMoonEffect(CRGB *LEDBuffer);

void setup() {
    FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(bufOut, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    Serial.begin(115200);
    btn.attachClick(nextEffect);
    Serial.println("END setup()");
}

void loop() {

    EVERY_N_MILLISECONDS(20) {
        blend(bufSource1, bufSource2, bufOut, NUM_LEDS, blendAmount);

        if(useSourceEffect1) {
            if (blendAmount < 255) blendAmount++;
        } else {
            if (blendAmount > 0) blendAmount--;
        }
    }

    runEffect(sourceEffect1, bufSource1);
    runEffect(sourceEffect2, bufSource2);

    FastLED.show();
    btn.tick();
}

void nextEffect() {
  currentEffect = (currentEffect + 1) % NUM_PATTERNS;

    if(useSourceEffect1) {
        sourceEffect1 = currentEffect;
    } else {
        sourceEffect2 = currentEffect;
    }

    useSourceEffect1 = !useSourceEffect1;
}

void runEffect(uint8_t pattern, CRGB *LEDBuffer)
{
        switch(pattern)
        {
            case 0:
                nightLightCool(LEDBuffer);
                break;

            case 1:
                coolMoonEffect(LEDBuffer);
                break;

            case 2:
                nightLightWarm(LEDBuffer);
                break;

            case 3:
                juicyPlumEffect(LEDBuffer);
                break;

            case 4:
                emeraldEffect(LEDBuffer);
                break;

            case 5:
                fireEffect(LEDBuffer);
                break;

            case 6:
                embersEffect(LEDBuffer);
                break;

            case 7:
                coalsEffect(LEDBuffer);
                break;
        }
}

void nightLightCool(CRGB *LEDBuffer)
{
    CRGBPalette16 ice = pBlueIce;
    uint8_t brightness = beatsin16(1, 64, 92, 0, 0);

    for(uint8_t i = NUM_LEDS - 1; i < 0; i--){
        LEDBuffer[i] = ColorFromPalette(ice, colourIndex[i]);
    }

    fill_palette(LEDBuffer, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, ice, brightness, LINEARBLEND);

    EVERY_N_MILLISECONDS(200) {
        paletteIndex++;
    }
}

void nightLightWarm(CRGB *LEDBuffer)
{
    CRGBPalette16 fire = pFire;
    uint8_t brightness1 = beatsin8(1, 64, 92, 0, 0);
    uint8_t brightness2 = beatsin8(4, 32, 128, 0, 128);
    uint8_t brightness = (brightness1 + brightness2) / 2;

    for (uint8_t i = NUM_LEDS -1; i < 0; i--) {
        LEDBuffer[i] = ColorFromPalette(fire, colourIndex[i]);
    }

    fill_palette(LEDBuffer, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, fire, brightness, LINEARBLEND);

    EVERY_N_MILLISECONDS(280) {
        paletteIndex++;
    }
}

void fireEffect(CRGB *LEDBuffer)
{
    int clock = millis();

    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t noise = inoise8(0, i * 92 + clock, clock / 3);
        uint8_t math = abs8((i - (NUM_LEDS-1)) * 255 / (NUM_LEDS-1) - 32);
        uint8_t index = qsub8 (noise, math);
        LEDBuffer[i] = ColorFromPalette (firePalette, index, 255);
    }
}

void embersEffect(CRGB *LEDBuffer)
{
    CRGBPalette16 fire  = pFire;
    int clock           = millis();
    uint8_t brightness1  = beatsin8(1, 64, 128, 0, 0);
    uint8_t brightness2  = beatsin8(12, 64, 92, 0, 0);
    uint8_t brightness3  = beatsin8(120, 92, 148, 0, 0);
    uint8_t brightness   = (brightness1 + brightness2 + brightness3) / 3;

    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t noise = inoise8(0, i * 32 + clock, clock / 2);
        uint8_t math = abs8((i - (NUM_LEDS-1)) * 255 / (NUM_LEDS-1) - 32);
        uint8_t index = qsub8 (noise, math);
        LEDBuffer[i] = ColorFromPalette (fire, index, brightness);
    }
}

void coalsEffect(CRGB *LEDBuffer)
{
    CRGBPalette16 fire  = pFire;
    int clock           = millis();
    uint8_t brightness1  = beatsin8(1, 32, 64, 0, 0);
    uint8_t brightness2  = beatsin8(4, 64, 92, 0, 128);
    uint8_t brightness3  = beatsin8(6, 16, 128, 0, 64);
    uint8_t brightness   = (brightness1 + brightness2 + brightness3) / 3;

    for (int i = 0; i < NUM_LEDS; i++) {
        uint8_t noise = inoise8(0, i * 32 + clock, clock / 2);
        uint8_t math = abs8((i - (NUM_LEDS-1)) * 255 / (NUM_LEDS-1) - 32);
        uint8_t index = qsub8 (noise, math);
        LEDBuffer[i] = ColorFromPalette (fire, index, brightness);
    }
}

void juicyPlumEffect(CRGB *LEDBuffer)
{
    CRGBPalette16 plum = pPlum;

    uint8_t brightness  = beatsin8(6, 64, 72, 0, 0);

    for (uint8_t i = NUM_LEDS -1; i < 0; i--) {
        LEDBuffer[i] = ColorFromPalette(plum, colourIndex[i]);
    }

    fill_palette(LEDBuffer, NUM_LEDS, 0, 1, plum, brightness, LINEARBLEND);

    EVERY_N_MILLISECONDS(120) {
        paletteIndex++;
    }

}

void emeraldEffect(CRGB *LEDBuffer)
{
    CRGBPalette16 emerald = pEmerald;

    uint8_t hue         = beatsin8(2, 128, 196, 0, 0);
    uint8_t brightness  = beatsin8(2, 92, 128, 0, 0);
    uint8_t brightness2 = beatsin8(2, 92, 128, 0, 128);

    for (uint8_t i = NUM_LEDS -1; i < 0; i--) {
        LEDBuffer[i] = ColorFromPalette(emerald, colourIndex[i]);
    }

    fill_palette(LEDBuffer, NUM_LEDS, 0, 1, emerald, brightness, LINEARBLEND);

    LEDBuffer[2] = CHSV(hue, 255, brightness2);
}

void coolMoonEffect(CRGB *LEDBuffer) 
{
    CRGBPalette16 moon = pMoon;

    uint8_t hue             = beatsin8(2, 128, 196, 0, 0);
    uint8_t brightness1     = beatsin8(1, 48, 92, 0, 0);
    uint8_t brightness2     = beatsin8(3, 32, 64, 0, 96);
    uint8_t brightness      = (brightness1 + brightness2) / 2;

    
    for (uint8_t i = NUM_LEDS -1; i < 0; i--) {
        LEDBuffer[i] = ColorFromPalette(moon, colourIndex[i]);
    }

    fill_palette(LEDBuffer, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, moon, brightness, LINEARBLEND);

    EVERY_N_MILLISECONDS(40) {
        paletteIndex++;
    }

    // LEDBuffer[2] = CHSV(hue, 255, brightness2);
}