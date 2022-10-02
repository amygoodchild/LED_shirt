/* 
 *  LED test
 *  
 *  Will show a nice fadey animation on two lines of LEDs.
 *  The strips must have 48 leds each respectively.
 *  
 */

// These are some settings for fastLED to work with the esp8266
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>
FASTLED_USING_NAMESPACE

// These are some settings to tell fastLED what kind of LEDs you have
// and how we want them to run in terms of brightness etc. 
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS         80
#define FRAMES_PER_SECOND  120

// There are two lines of LEDs, A and B.

// These are the pins for each of them
#define LED_PIN_A   D3
#define LED_PIN_B   D4

// This is the number of LEDs in each line 
#define NUM_LEDS_A    48
#define NUM_LEDS_B    48

// In total there are 100 LEDs, each LED can be thought of as having an ID of 0-96.
#define NUM_LEDS 96

// These variables store the ID of the first LED in each line.
// A starts at 0 (and goes up to 29)
// Therefore B starts at 30 (and goes up to 57) etc etc
#define START_LEDS_A    0
#define START_LEDS_B    48

// This creates an array of the LEDs
CRGB leds[NUM_LEDS];

// // //

// These variables are for the animation
// During the animation, we move down each line, lighting up each LED in turn
// (We then also slightly darken every LED in the line, so that only LEDs that have been lit up recently look bright)
// These variables hold ID of the LED we're lighting up this turn, 
// here we set them all to start with the first ID in that line.
float lightA = START_LEDS_A;
float lightB = START_LEDS_B;

void setup() {

  // FAST LED STUFF
  // Tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,LED_PIN_A,COLOR_ORDER>(leds, START_LEDS_A, NUM_LEDS_A).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,LED_PIN_B,COLOR_ORDER>(leds, START_LEDS_B, NUM_LEDS_B).setCorrection(TypicalLEDStrip);

  // Set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void loop() {

  // Call the animation function
  pinkAndBlue();

  // Send the 'leds' array out to the actual LED strip and make it show up!
  FastLED.show(); 
  
  // Insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  
}

void pinkAndBlue(){

  // In every frame, we increment the variables, 
  // so that the LED we're lighting up will move down the line
  lightA += 0.20;
  lightB += 0.20;

  // If we've reached the end of a line, go back to the beginning
  if (lightA > START_LEDS_B){  
    lightA = START_LEDS_A;
  }
  
  if (lightB >= NUM_LEDS){ 
    lightB = START_LEDS_B;
  }
  
  // Go through ALL the leds, and fade them towards black/dark a bit
  fadeToBlackBy( leds, NUM_LEDS, 20);

  // Light up the current chosen LED in each line, each line does a slightly different colour for fun
  // These colours are in hue, saturation, brightness format (each out of 255) 
  // E.g. in the first one, 247 is the hue, 151 is the saturation, 125 is brightness.
  leds[(int)lightA] += CHSV( 247, 151, 125);
  leds[(int)lightB] += CHSV( 203, 200, 125);
    
}
