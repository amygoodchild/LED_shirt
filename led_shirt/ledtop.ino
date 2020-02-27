/* LED top
 *  
 */

#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ESP8266_D1_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0

#include <FastLED.h>
FASTLED_USING_NAMESPACE

#include <Wire.h> 
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
Adafruit_7segment matrix = Adafruit_7segment();


#define DATA_PIN_A   D3
#define DATA_PIN_B   D4
#define DATA_PIN_C   D5
#define DATA_PIN_D   D6

#define BUTTON_PIN D0
int buttonState = 0;
int counter = 0;
bool offed = true;

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS_A    30
#define NUM_LEDS_B    28
#define NUM_LEDS_C    12
#define NUM_LEDS_D    30

#define START_LEDS_A    0
#define START_LEDS_B    30
#define START_LEDS_C    58
#define START_LEDS_D    70

// Color variables
float lightA = START_LEDS_A;
float lightB = START_LEDS_B;
float lightC = START_LEDS_C;
float lightD = START_LEDS_D;

#define NUM_LEDS 100

CRGB leds[NUM_LEDS];

#define BRIGHTNESS         100
#define FRAMES_PER_SECOND  120



void setup() {

#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
#endif
  matrix.begin(0x70);

  pinMode(BUTTON_PIN, INPUT);

  // FAST LED STUFF
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN_A,COLOR_ORDER>(leds, START_LEDS_A, NUM_LEDS_A).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_B,COLOR_ORDER>(leds, START_LEDS_B, NUM_LEDS_B).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_C,COLOR_ORDER>(leds, START_LEDS_C, NUM_LEDS_C).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<LED_TYPE,DATA_PIN_D,COLOR_ORDER>(leds, START_LEDS_D, NUM_LEDS_D).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

 
}


void loop() {
  

  pinkAndBlue();

  // send the 'leds' array out to the actual LED strip
  FastLED.show(); 
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  buttonState = digitalRead(BUTTON_PIN);
  

  if (buttonState == HIGH) {
    Serial.println("ON");
    if (offed){
      counter++;
    }
    offed = false; 
  } else {
    Serial.println("OFF");
    offed = true;
    matrix.writeDisplay();
  }
  

  matrix.writeDigitNum(4, counter, false);

  if (counter > 9){
    counter = 0;
  }
  
  
}


void pinkAndBlue(){

  lightA+= 0.20;
  lightB+= 0.20;
  lightC+= 0.20;
  lightD+= 0.20;
  
  if (lightA > START_LEDS_B){  
    lightA = START_LEDS_A;
  }
  if (lightB >= START_LEDS_C){ 
    lightB = START_LEDS_B;
  }
  
  if (lightC >= START_LEDS_D){ 
    lightC = START_LEDS_C;
  }
  
  if (lightD >= NUM_LEDS){ 
    lightD = START_LEDS_D;
  }
  
  fadeToBlackBy( leds, NUM_LEDS, 20);

  leds[(int)lightA] += CHSV( 247, 151, 125);
  leds[(int)lightB] += CHSV( 203, 200, 125);
  leds[(int)lightC] += CHSV( 112, 130, 125); 
  leds[(int)lightD] += CHSV( 235, 190, 255);
    
}
